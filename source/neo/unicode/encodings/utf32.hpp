#ifndef NEO_UNICODE_ENCODINGS_UTF32_HPP_INCLUDED
#define NEO_UNICODE_ENCODINGS_UTF32_HPP_INCLUDED

#include <neo/unicode/code_unit_buffer.hpp>

#include "encodings.hpp"

namespace neo {

inline namespace encodings {

struct utf32 {
    using code_unit_type = char32_t;
    using buffer_type = neo::code_unit_buffer<code_unit_type>;

    static const char* get_name() noexcept {
        return "utf-32";
    }
};

template <> struct encoding_for_char_type<char32_t> { using type = utf32; };

template <> struct encoder<struct utf8, utf32> {
    template <typename FromBuffer> static utf32::buffer_type encode(const FromBuffer&);
};

}  // namespace encodings

}  // namespace neo

#endif // NEO_UNICODE_ENCODINGS_UTF32_HPP_INCLUDED