#ifndef NEO_UNICODE_UNICODE_HPP_INCLUDED
#define NEO_UNICODE_UNICODE_HPP_INCLUDED

#include "text.hpp"

#include "encodings/utf8.hpp"

namespace neo {

using unicode = basic_text<encodings::utf8>;

inline namespace literals {

inline unicode
operator""_u(const char* ptr,
             std::size_t len) noexcept(noexcept(unicode::from_string_literal(ptr, len))) {
    return unicode::from_string_literal(ptr, len);
}

}  // literals

}  // namespace neo

#endif  // NEO_UNICODE_UNICODE_HPP_INCLUDED
