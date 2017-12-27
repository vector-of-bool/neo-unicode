#ifndef NEO_UNICODE_ENCODINGS_UTF16_HPP_INCLUDED
#define NEO_UNICODE_ENCODINGS_UTF16_HPP_INCLUDED

#include <neo/unicode/code_unit_buffer.hpp>

#include "encodings.hpp"

namespace neo {

inline namespace encodings {

struct utf16 {
    using code_unit_type = char16_t;
    using buffer_type = neo::code_unit_buffer<code_unit_type>;

    static const char* get_name() noexcept {
        return "utf-16";
    }
};

template <> struct encoding_for_char_type<char16_t> { using type = utf16; };

}  // namespace encodings

}  // namespace neo

#endif  // NEO_UNICODE_ENCODINGS_UTF16_HPP_INCLUDED