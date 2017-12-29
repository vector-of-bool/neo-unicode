#ifndef NEO_UNICODE_ENCODINGS_UTF16_HPP_INCLUDED
#define NEO_UNICODE_ENCODINGS_UTF16_HPP_INCLUDED

#include <neo/unicode/code_unit_buffer.hpp>

#include "encodings.hpp"
#include "utf8.hpp"

namespace neo {

inline namespace encodings {

struct utf16 {
    using code_unit_type = char16_t;
    using buffer_type = neo::code_unit_buffer<code_unit_type>;

    static const char* get_name() noexcept { return "utf-16"; }
};

template <> struct encoding_for_char_type<char16_t> { using type = utf16; };

template <> struct encoder<utf8, utf16> {
    static utf16::buffer_type do_encode(const char* ptr, std::size_t);

    template <typename FromBuffer> static utf16::buffer_type encode(FromBuffer&& buf) {
        return do_encode(buf.data(), buf.code_unit_size());
    }
};

template <> struct encoder<utf16, utf8> {
    static utf8::buffer_type do_encode(const char16_t* ptr, std::size_t);
    template <typename FromBuffer> static utf8::buffer_type encode(FromBuffer&& buf) {
        return do_encode(buf.data(), buf.code_unit_size());
    }
};

}  // namespace encodings

}  // namespace neo

#endif  // NEO_UNICODE_ENCODINGS_UTF16_HPP_INCLUDED