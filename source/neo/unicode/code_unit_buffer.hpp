#ifndef NEO_UNICODE_CODE_UNIT_BUFFER_HPP_INCLUDED
#define NEO_UNICODE_CODE_UNIT_BUFFER_HPP_INCLUDED

#include <atomic>
#include <cassert>
#include <cinttypes>
#include <iterator>
#include <memory>

#include "concepts.hpp"

namespace neo {

/**
 * The maximum number of code units in a code_unit_buffer for which a
 * small-string optimization shall occur.
 */
static constexpr std::size_t small_size = 32;

/**
 * `neo::code_unit_buffer` is a class template for working with sequences of
 * code units. It manages allocation and the like.
 *
 * @tparam T (CodeUnit) The type used to store code units. For UTF-8, this would
 * be unsigned char. UTF-16 might use char16_t.
 * @tparam Allocator The allocator type. Won't be used in some cases.
 *
 * `code_unit_buffer` does some magic tricks to minimize allocation:
 *
 * 1. Small-string optimization: If the string is fewer than `small_size`
 *      code units, the string is stored in a member array.
 * 2. Literal references: If the a unicode object is constructed from a string
 *      literal, `code_unit_buffer` will store a pointer to the string literal,
 *      which can be of arbitrary size.
 * 3. Immutability: The underlying bytes are *immutable* and shared. Copying a
 *      `code_unit_buffer` will not copy the bytes, just increase a refcount to
 *      the allocated buffer. This makes copying `unicode` objects extremely
 *      cheap, even for many large strings.
 */
template <neo_concept_param(CodeUnit) T, typename Allocator = std::allocator<char>>
class code_unit_buffer {
    /**
     * The traits for the allocator
     */
    using Alloc = std::allocator_traits<Allocator>;
    using CodeUnitAllocator = typename Alloc::template rebind_alloc<T>;
    using CodeUnitAlloc = std::allocator_traits<CodeUnitAllocator>;

    /**
     * Check our concepts if we aren't on a recent GCC
     */
    static_assert(CodeUnit<T>, "T does not meet CodeUnit requirements");

    /**
     * We don't expose a non-const pointer in our public interface, since the
     * code units are immutable
     */
    using NonConstPointer = typename CodeUnitAlloc::pointer;

public:
    /**
     * The actual type used to store the code units
     */
    using value_type = T;
    /**
     * The allocator type
     */
    using allocator_type = Allocator;
    /**
     * Type suitable for respresenting the length of a code unit sequence
     */
    using size_type = typename CodeUnitAlloc::size_type;
    /**
     * Type suitable for representing the distance between two pointers to code
     * units.
     */
    using difference_type = typename CodeUnitAlloc::difference_type;
    /**
     * Reference to a code unit.
     */
    using reference = const value_type&;
    using const_reference = reference;
    /**
     * Type used to point to code units
     */
    using pointer = typename CodeUnitAlloc::const_pointer;
    using const_pointer = typename CodeUnitAlloc::const_pointer;
    /**
     * Iterator for iterating over code units.
     */
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    // `code_unit_buffer` has a few underlying representations, which are toggled between
    // depending on how the `code_unit_buffer` was initialized. They are detailed below

    // `literal_data` is for when `code_unit_buffer` is initialized from a string
    // literal. We assume that all string literals are properly encoded. This
    // initialization lends itself to optimization.
    struct literal_data {
        const_pointer pointer;
        size_type size;
    };
    // `sso_data` is for the familiar small-string optimization. It stores the
    // strings smaller than `small_size`, but no larger.
    struct sso_data {
        value_type arr[small_size];
        size_type size;
    };
    // `dynamic_data` is for the case where we need to dynamically allocate the
    // contents of the string. We use reference counting to prevent unnecessary
    // allocation.
    struct dynamic_data {
        // `refs` is the reference counter
        std::atomic<std::size_t> refs;
        // Size is the length of the string
        size_type size;
        // The buffer lies here
        value_type arr[1];
    };

    /**
     * A union we use to store information about code units. Each field
     * corresponds to a differen representation mode.
     */
    union buffer_content {
        literal_data literal;
        sso_data small;
        dynamic_data* dynamic;
    };
    /**
     * Here it is! The actual data storage.
     */
    buffer_content _content;
    /**
     * Enum corresponding to the different representation modes
     */
    enum mode {
        literal,
        small,
        dynamic,
    };
    /**
     * The current mode. We start in small mode. The default constructor sets
     * us to an empty sequence
     */
    mode _mode = small;

    /**
     * The allocator. TODO: Use EBO on this
     */
    allocator_type _alloc;

    char* _dynamic_as_char_ptr() {
        return reinterpret_cast<char*>(_content.dynamic);
    }

    /**
     * Release the dynamic data. If we are the last reference, we deallocate it.
     * @pre: _mode == dynamic
     */
    void _release_dynamic() noexcept {
        assert(_mode == dynamic);
        auto old_refs = _content.dynamic->refs.fetch_sub(1);
        if (old_refs == 1) {
            // No more references to our dynamic data. Free the data
            Alloc::deallocate(_alloc,
                              _dynamic_as_char_ptr(),
                              // The size hint. Add the size of the string.
                              (_content.dynamic->size + sizeof(value_type)) + sizeof(dynamic_data));
            // Reset the dynamic data, we aren't using it anymore
            _content.dynamic = nullptr;
        }
        _mode = small;
        _content.small.size = 0;
        _content.small.arr[0] = value_type(0);
    }

    /**
     * Tag type used to force overload to build from a string literal
     */
    struct from_literal_tag {};

    /**
     * Construct from a string literal. This is private because we don't want
     * users to accidentally do this.
     * @param ptr The pointer to a properly encoded sequence of code units
     * @param len The length of the string referenced by `prt`
     */
    code_unit_buffer(pointer ptr, size_type len, from_literal_tag) noexcept {
        _mode = literal;
        _content.literal.pointer = ptr;
        _content.literal.size = len;
    }

    dynamic_data* _steal_dynamic() && {
        assert(_mode == dynamic);
        const auto ptr = _content.dynamic;
        _content.dynamic = nullptr;
        _mode = small;
        _content.small.size = 0;
        _content.small.arr[0] = value_type(0);
        return ptr;
    }

    void _init_dynamic(size_type size) {
        // We use a old C trick to allocate a single buffer to hold a struct
        // with a trailing array. We add the size of the struct, plus the
        // size of the string to store.
        // NOTE: We DO NOT add +1 to the size for the null terminator,
        // because the struct array already has a length of 1 to start with,
        // since zero-sized arrays are not valid C++. We use the extra
        // element to store the null terminator.
        _content.dynamic
            = reinterpret_cast<dynamic_data*>(Alloc::allocate(_alloc, sizeof(dynamic_data) + (size * sizeof(value_type))));
        // Initialize the dynamic data
        _content.dynamic->refs = 1;                   // One reference
        _content.dynamic->size = size;                // Size of string
        _content.dynamic->arr[size] = value_type(0);  // Add null terminator
    }

    value_type* _prepare_storage(size_type size) {
        if (size < small_size) {
            // We're small enough to fit!
            // NO: Using less-than is *not a typo*. We need one extra code unit
            // to store the null terminator.
            _mode = small;
            _content.small.size = size;
            // Put the null terminator on the end
            _content.small.arr[size] = value_type(0);
            return _content.small.arr;
        } else {
            // Not enough space. Allocation time!
            _mode = dynamic;
            _init_dynamic(size);
            return _content.dynamic->arr;
        }
    }

public:
    /**
     * Default construct to an empty sequence. Uses the SSO to no-alloc
     */
    code_unit_buffer() noexcept {
        _content.small.size = 0;
        // Set the null terminator!
        _content.small.arr[0] = value_type(0);
    }

    /**
     * Destroy the buffer. Only releases the dynamic data, if needed
     */
    ~code_unit_buffer() noexcept {
        if (_mode == dynamic && _content.dynamic) {
            _release_dynamic();
        }
    }

    /**
     * The array constructor.
     *
     * This shouldn't be invoked manually: It should happen implicitly from a
     * string literal.
     */
    template <std::size_t N>
    explicit code_unit_buffer(const value_type (&arr)[N]) noexcept
        : code_unit_buffer(arr, N - 1, from_literal_tag{}) {
    }

    /**
     * The range constructor. Copies the code units referred to by [first, last)
     * into the internal buffer. May use SSO if the range is small enough,
     * otherwise allocates.
     *
     * @param first The start of the range
     * @param last The end of the range
     * @param alloc The allocator to use
     */
    template <typename Iterator>
    code_unit_buffer(Iterator first, Iterator last, allocator_type alloc = allocator_type())
        : _alloc(alloc) {
        const auto size = std::distance(first, last);
        auto wr_ptr = _prepare_storage(size);
        // Copy in the data:
        while (first != last) {
            *wr_ptr = *first;
            ++wr_ptr;
            ++first;
        }
    }

    explicit code_unit_buffer(const_pointer ptr, allocator_type alloc = allocator_type())
        : code_unit_buffer(ptr, ptr + std::char_traits<value_type>::length(ptr), alloc) {}

    /**
     * Copy the buffer. Defined in terms of copy-assignment
     */
    code_unit_buffer(const code_unit_buffer& other) noexcept {
        *this = other;
    }

    /**
     * Move from the other guy. We can optimize here by not performing the
     * reference counting. If the other string is not dynamic, we just defer
     * to copy-assignment
     */
    code_unit_buffer(code_unit_buffer&& other) noexcept {
        if (other._mode == dynamic) {
            // Steal their dynamic pointer. No need to do any reference counting
            _content.dynamic = std::move(other)._steal_dynamic();
            _mode = dynamic;
        } else {
            // Just do a regular copy
            *this = other;
        }
    }

    /**
     * Copy from another buffer
     */
    code_unit_buffer& operator=(const code_unit_buffer& other) noexcept {
        // If we are a dynamic buffer, release our data
        if (_mode == dynamic) {
            _release_dynamic();
        }
        // Take the other allocator
        _alloc = Alloc::select_on_container_copy_construction(other._alloc);
        if (other._mode == dynamic) {
            // Take the other ones dynamic data and add a reference
            _content.dynamic = other._content.dynamic;
            _content.dynamic->refs++;
        } else if (other._mode == literal) {
            // Simple
            _content.literal = other._content.literal;
        } else {
            // Simple
            _content.small = other._content.small;
        }
        // Take their mode
        _mode = other._mode;
        return *this;
    }

    /**
     * Move from another buffer. Mostly identical to copy-assignment, except we
     * do a simple steal in the case of dynamic data.
     */
    code_unit_buffer& operator=(code_unit_buffer&& other) noexcept {
        if (_mode == dynamic) {
            _release_dynamic();
        }
        _alloc = Alloc::select_on_container_copy_construction(other._alloc);
        if (other._mode == dynamic) {
            _content.dynamic = std::move(other)._steal_dynamic();
            _mode = dynamic;
        } else {
            *this = other;
        }
        return *this;
    }

    /**
     * Get a pointer to the underlying buffer of code units.
     */
    const_pointer data() const noexcept {
        switch (_mode) {
        case dynamic:
            return _content.dynamic->arr;
        case small:
            return _content.small.arr;
        case literal:
            return _content.literal.pointer;
        }
        std::terminate();
    }

    /**
     * Get the number of bytes in the sequence. Not the number of code units!
     */
    size_type byte_size() const noexcept {
        return code_unit_size() * sizeof(value_type);
    }

    /**
     * Get the number of code units in the sequence. Not the number of bytes!
     */
    size_type code_unit_size() const noexcept {
        switch (_mode) {
        case dynamic:
            return _content.dynamic->size;
        case small:
            return _content.small.size;
        case literal:
            return _content.literal.size;
        }
        std::terminate();
    }

    /**
     * Construct from a string literal. Uses the named-constructor idiom to make
     * this explicit. `neo::basic_text` will also do the implicit conversion.
     */
    static code_unit_buffer from_string_literal(pointer ptr, size_type len) noexcept {
        return code_unit_buffer{ptr, len, from_literal_tag{}};
    }

    template <typename Filler> static code_unit_buffer fill(size_type len, Filler&& fn) {
        code_unit_buffer ret;
        auto wr_ptr = ret._prepare_storage(len);
        fn(wr_ptr);
        return ret;
    }
};

using utf8_buffer = code_unit_buffer<char>;
using utf16_buffer = code_unit_buffer<char16_t>;
using utf32_buffer = code_unit_buffer<char32_t>;
using wchar_buffer = code_unit_buffer<wchar_t>;

}  // namespace neo

#endif  // NEO_UNICODE_CODE_UNIT_BUFFER_HPP_INCLUDED