#include "utf16.hpp"

#include <utf8rewind.h>

neo::utf16::buffer_type neo::encoder<neo::utf8, neo::utf16>::do_encode(const char* ptr, std::size_t size) {
    std::int32_t errors = 0;
    const auto req_size = ::utf8toutf16(ptr, size, nullptr, 0, &errors) / sizeof(char16_t);
    if (req_size == 0 || errors != UTF8_ERR_NONE) {
        throw std::runtime_error("??");  // todo. Probably define an error_category
    }

    return neo::utf16::buffer_type::fill(req_size, [&](auto dest_) {
        const auto dest = reinterpret_cast<::utf16_t*>(dest_);
        ::utf8toutf16(ptr, size, dest, req_size * sizeof(char16_t), nullptr);
    });
}

neo::utf8::buffer_type neo::encoder<neo::utf16, neo::utf8>::do_encode(const char16_t* ptr_, std::size_t size) {
    const auto ptr = reinterpret_cast<const ::utf16_t*>(ptr_);
    std::int32_t errors = 0;
    const auto req_size = ::utf16toutf8(ptr, size * sizeof(char16_t), nullptr, 0, &errors);
    if (req_size == 0 || errors != UTF8_ERR_NONE) {
        throw std::runtime_error("??");  // todo. Probably define an error_category
    }

    return neo::utf8::buffer_type::fill(req_size, [&](auto dest) {
        ::utf16toutf8(ptr, size * sizeof(char16_t), dest, req_size, nullptr);
    });
}