#ifndef NEO_UNICODE_ENCODINGS_ENCODINGS_HPP_INCLUDED
#define NEO_UNICODE_ENCODINGS_ENCODINGS_HPP_INCLUDED

namespace neo {

inline namespace encodings {

template <typename Char>
struct encoding_for_char_type;

template <typename Char>
using encoding_for_char_type_t = typename encoding_for_char_type<Char>::type;

template <typename FromEncoding, typename ToEncoding> struct encoder;

} // namespace encodings

} // namespace neo

#endif // NEO_UNICODE_ENCODINGS_ENCODINGS_HPP_INCLUDED