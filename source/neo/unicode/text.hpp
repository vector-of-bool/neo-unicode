#ifndef NEO_UNICODE_TEXT_HPP_INCLUDED
#define NEO_UNICODE_TEXT_HPP_INCLUDED

#include "concepts.hpp"
#include "encodings/all.hpp"

#include <string>

namespace neo {

/**
 * A type wrapping a text buffer. The internal encoding is opaque.
 *
 * @tparam InternalEncoding An `Encoding` that defines the internal encoding
 *  used by this text object.
 * @tparam BufferType A `CodeUnitBuffer` that defines the type used to store
 *  the data. Deduced from the `InternalEncoding`.
 */
template <neo_concept_param(Encoding) InternalEncoding,
          neo_concept_param(CodeUnitBuffer) BufferType = buffer_type_t<InternalEncoding>>
class basic_text {
public:
    /**
     * The internal encoding used by this text object
     */
    using internal_encoding = InternalEncoding;
    /**
     * The buffer type used to store the text data
     */
    using buffer_type = BufferType;
    /**
     * The type of allocator used by `buffer_type`
     */
    using allocator_type = typename buffer_type::allocator_type;

    /**
     * The code-unit pointer type used by `buffer_type`
     */
    using pointer = pointer_t<buffer_type>;
    using const_pointer = const_pointer_t<buffer_type>;
    /**
     * The `size_type` used by `buffer_type`
     */
    using size_type = size_type_t<buffer_type>;

    // Check that the value type of the buffer is convertible to the type used
    // by the encoding class
    static_assert(unicode_detail::Convertible<value_type_t<BufferType>,
                                              typename internal_encoding::code_unit_type>,
                  "Encoding and buffer type are not compatible");

private:
    /**
     * The actual data buffer
     */
    buffer_type _buffer;

public:
    /**
     * Default constructor. Constructs an empty text.
     */
    basic_text() = default;

    /**
     * Literal constructor. Implicitly convertible from a string literal.
     */
    template <std::size_t N>
    basic_text(const value_type_t<buffer_type> (&arr)[N]) noexcept(noexcept(buffer_type(arr)))
        : _buffer(arr) {
    }

    /**
     * Character pointer constructor. This constructs from a pointer to a sequence
     * of code units, probably obtained via some other means
     */
    template <typename CharPointer,
              typename = std::enable_if_t<std::is_convertible<CharPointer, const_pointer>::value>>
    basic_text(const CharPointer ptr) noexcept(noexcept(buffer_type(ptr, ptr)))
        : _buffer(ptr, ptr + std::char_traits<value_type_t<buffer_type>>::length(ptr)) {
    }

    /**
     * Obtain a pointer to the underlying encoded code unit sequence
     */
    const_pointer data() const noexcept(noexcept(_buffer.data())) {
        return _buffer.data();
    }

    /**
     * Get the number of bytes in the buffer. Not the number of code units
     */
    size_type byte_size() const noexcept(noexcept(_buffer.byte_size())) {
        return _buffer.byte_size();
    }

    /**
     * Get the number of code units in the buffer. Not the number of bytes.
     */
    size_type code_unit_size() const noexcept(noexcept(_buffer.code_unit_size())) {
        return _buffer.code_unit_size();
    }

    /**
     * Name constructor for creating from a string literal
     */
    static basic_text from_string_literal(pointer ptr, size_type len) noexcept(
        noexcept(buffer_type::from_string_literal(ptr, len))) {
        basic_text ret;
        ret._buffer = buffer_type::from_string_literal(ptr, len);
        return ret;
    }

private:
    template <typename T> struct tag {};

    const buffer_type& _encode(tag<internal_encoding>) const {
        return _buffer;
    }

    template <typename DestEncoding>
    buffer_type_t<DestEncoding> _encode(tag<DestEncoding>) const {
        return DestEncoding::template encode_from<internal_encoding>(_buffer);
    }

public:
    template <typename NewEncoding>
    decltype(auto) encode() const {
        return _encode(tag<NewEncoding>{});
    }
};

/**
 * Stream ouput for text objects.
 */
template <typename CharT, typename Traits, typename InternalEncoding, typename BufferType>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& o,
                                              const basic_text<InternalEncoding, BufferType>& u) {
    auto buf = u.template encode<neo::encoding_for_char_type_t<CharT>>();
    auto size = buf.code_unit_size();
    return o.write(buf.data(), size);
}

}  // namespace neo

#endif  // NEO_UNICODE_TEXT_HPP_INCLUDED