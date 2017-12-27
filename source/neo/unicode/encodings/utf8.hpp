#ifndef NEO_UNICODE_ENCODINGS_UTF8_HPP_INCLUDED
#define NEO_UNICODE_ENCODINGS_UTF8_HPP_INCLUDED

#include <neo/unicode/code_unit_buffer.hpp>

#include "encodings.hpp"

namespace neo {

inline namespace encodings {

struct utf8 {
    using code_unit_type = char;
    using buffer_type = neo::code_unit_buffer<code_unit_type>;

    static const char* get_name() noexcept {
        return "utf-8";
    }
};

template <> struct encoding_for_char_type<char> { using type = utf8; };

}  // namespace encodings

}  // namespace neo

#endif  // NEO_UNICODE_ENCODINGS_UTF8_HPP_INCLUDED