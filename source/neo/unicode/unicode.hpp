#ifndef NEO_UNICODE_HPP_INCLUDED
#define NEO_UNICODE_HPP_INCLUDED

#include <atomic>
#include <cassert>
#include <cstring>
#include <memory>
#include <ostream>
#include <string>


namespace neo {

class unicode;
class bytes;

struct utf8_t {
    static bytes encode(const unicode& u);
    template <typename Allocator = std::allocator<char>>
    static inline unicode decode(const char* first, const char* last, Allocator = Allocator());
};

extern utf8_t utf8;

struct utf16_t {
    static bytes encode(const unicode& u);
    static unicode decode(const char* first, const char* last);
};

extern utf16_t utf16;

template <typename Char> struct decoder_for;

template <> struct decoder_for<char> { using type = utf8_t; };

template <typename T> using decoder_for_t = typename decoder_for<T>::type;

static constexpr std::size_t sso_size = 32;

class unicode {
    // `unicode` has a few underlying representations, which are toggled between
    // depending on how the `unicode` was initialized. They are detailed below

    // `literal_data` is for when `unicode` is initialized from a string
    // literal. We assume that all string literals are encoded in utf-8. This
    // initialization lends itself to optimization.
    struct literal_data {
        const char* pointer;
        std::size_t size;
    };
    // `sso_data` is for the familiar small-string optimization. It stores the
    // strings smaller than `sso_size`, but no larger.
    struct sso_data {
        char arr[sso_size];
        std::size_t size;
    };
    // `dynamic_data` is for the case where we need to dynamically allocate the
    // contents of the string. We use reference counting to prevent unnecessary
    // allocation.
    struct dynamic_data {
        // `refs` is the reference counter
        std::atomic_uint refs;
        // Size is the length of the string
        std::size_t size;
        // And an array
        char arr[1];
    };
    union unicode_content {
        literal_data literal;
        sso_data small;
        dynamic_data* dynamic;
    };
    unicode_content _content;
    enum mode {
        literal,
        small,
        dynamic,
    };
    mode _mode = small;

    unicode(const char* first, const char* last) {
        const auto size = std::distance(first, last);
        if (size <= sso_size) {
            _mode = small;
            _content.small.size = size;
            std::memcpy(_content.small.arr, first, size);
        } else {
            _mode = dynamic;
            // Allocate size of the struct plus the data array
            auto dyn_buf = new char[sizeof(dynamic_data) + size];
            auto& dyn = reinterpret_cast<dynamic_data&>(*dyn_buf);
            new (&dyn.refs) std::atomic_uint;
            dyn.refs = 1;
            dyn.size = size;
            dyn.arr[size] = '\0';
            std::memcpy(dyn.arr, first, size);
            _content.dynamic = &dyn;
        }
    }

    void _release_dynamic() {
        assert(_mode == dynamic);
        auto old_refs = _content.dynamic->refs.fetch_sub(1);
        if (old_refs == 1) {
            // No more references to our dynamic data. Free the data
            delete[] _content.dynamic;
            _content.dynamic = nullptr;
        }
    }

    struct from_literal_tag {};

    unicode(const char* ptr, std::size_t len, from_literal_tag) {
        _mode = literal;
        _content.literal.pointer = ptr;
        _content.literal.size = len;
    }

public:
    unicode() { _content.small.size = 0; }

    ~unicode() {
        if (_mode == dynamic) {
            _release_dynamic();
        }
    }

    unicode(const unicode& other) { *this = other; }

    unicode(unicode&& other) {
        if (_mode == dynamic) {
            _content.dynamic = other._content.dynamic;
            other._content.dynamic = nullptr;
        } else {
            *this = other;
        }
    }

    unicode& operator=(const unicode& other) {
        if (_mode == dynamic) {
            _release_dynamic();
        }
        if (other._mode == dynamic) {
            _content.dynamic = other._content.dynamic;
            _content.dynamic->refs++;
        } else if (other._mode == literal) {
            _content.literal = other._content.literal;
        } else {
            _content.small = other._content.small;
        }
        _mode = other._mode;
        return *this;
    }

    unicode& operator=(unicode&& other) {
        if (_mode == dynamic) {
            _release_dynamic();
        }
        if (other._mode == dynamic) {
            std::swap(_content.dynamic, other._content.dynamic);
        } else {
            *this = other;
        }
        _mode = other._mode;
        return *this;
    }

    template <typename Char, typename Traits, typename Allocator>
    unicode(const std::basic_string<Char, Traits, Allocator>& str)
        : unicode{ decoder_for_t<Char>::decode(str.data(), str.data() + str.size()) } {}

    template <std::size_t N>
    unicode(const char (&arr)[N])
        : unicode{ arr, N - 1, from_literal_tag{} } {}

    template <typename Char, std::size_t N>
    unicode(const Char (&arr)[N])
        : unicode{ decoder_for_t<Char>::decode(arr, arr + N - 1) } {}

    static unicode from_utf8(const char* first, const char* last) { return unicode{ first, last }; }

    template <typename Char> static unicode from_string_literal(const Char* first, std::size_t len) {
        return unicode{ first, len, from_literal_tag{} };
    }

private:
    template <typename Holds> class raw_view {
    public:
        using iterator = const char*;
        using const_iterator = iterator;

    private:
        Holds _owner;
        using IterTraits = std::iterator_traits<iterator>;

    public:
        using size_type = std::size_t;
        using difference_type = IterTraits::difference_type;

    public:
        raw_view(const Holds& u)
            : _owner(u) {}
        raw_view(Holds&& u)
            : _owner(std::move(u)) {}

        size_type size() const { return std::distance(begin(), end()); }

        iterator begin() const {
            const unicode& u = _owner;
            switch (u._mode) {
            case literal:
                return u._content.literal.pointer;
            case small:
                return u._content.small.arr;
            case dynamic:
                return u._content.dynamic->arr;
            }
            std::terminate();
        }

        iterator end() const {
            const auto first = begin();
            const unicode& u = _owner;
            switch (u._mode) {
            case literal:
                return first + u._content.literal.size;
            case small:
                return first + u._content.small.size;
            case dynamic:
                return first + u._content.dynamic->size;
            }
            std::terminate();
        }
    };

public:
    raw_view<std::reference_wrapper<const unicode>> raw() const & { return { *this }; }
    raw_view<unicode> raw() && { return std::move(*this); }

    // Encoding/decoding
    template <typename Encoder> auto encode(const Encoder& e) const -> decltype(e.encode(*this)) {
        return Encoder::encode(*this);
    }

    const char* data() const { return raw().begin(); }
};

template <typename Allocator> inline unicode utf8_t::decode(const char* first, const char* last, Allocator) {
    return unicode::from_utf8(first, last);
}

namespace literals {

inline unicode operator""_u(const char* ptr, std::size_t len) { return unicode::from_string_literal(ptr, len); }

}  // literals

inline std::ostream& operator<<(std::ostream& o, const unicode& u) {
    o << u.data();
    return o;
}

}  // neo

#endif  // NEO_UNICODE_HPP_INCLUDED