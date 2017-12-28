#ifndef NEO_UNICODE_ENCODINGS_WIDE_HPP_INCLUDED
#define NEO_UNICODE_ENCODINGS_WIDE_HPP_INCLUDED

#ifdef _WIN32
#include "utf16.hpp"
#else
#include "utf32.hpp"
#endif

namespace neo {

inline namespace encodings {

struct wide {
#ifdef _WIN32
    using underlying = utf16;
#else
    using underlying = utf32;
#endif
    using code_unit_type = wchar_t;
    using buffer_type = neo::code_unit_buffer<code_unit_type>;

    static const char* get_name() noexcept {
        return underlying::get_name();
    }

    template <typename FromEncoding, typename FromBuffer>
    static buffer_type encode_from(FromBuffer&& buf) {
        return encoder<FromEncoding, wide>::encode(buf);
    }
};

template <> struct encoding_for_char_type<wchar_t> { using type = wide; };

template <> struct encoder<struct utf8, wide> {
    static wide::buffer_type do_encode(const char*, std::size_t);
    template <typename FromBuffer> static wide::buffer_type encode(FromBuffer&& buf) {
        return do_encode(buf.data(), buf.code_unit_size());
    }
};

}  // namespace encodings

}  // namespace neo

#endif  // NEO_UNICODE_ENCODINGS_WIDE_HPP_INCLUDED