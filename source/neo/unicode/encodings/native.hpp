#ifndef NEO_UNICODE_ENCODINGS_NATIVE_HPP_INCLUDED
#define NEO_UNICODE_ENCODINGS_NATIVE_HPP_INCLUDED

#if defined(_WIN32) && (defined(_UNICODE) || defined(UNICODE))
#include "wide.hpp"
#else
#include "utf8.hpp"
#endif

namespace neo {

inline namespace encodings {

#if defined(_WIN32) && (defined(_UNICODE) || defined(UNICODE))
using native_encoding = wide;
#else
using native_encoding = utf8;
#endif

}  // namespace encodings

}  // namespace neo

#endif  // NEO_UNICODE_ENCODINGS_NATIVE_HPP_INCLUDED