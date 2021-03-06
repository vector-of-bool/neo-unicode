#include "wide.hpp"

#include <utf8rewind.h>

neo::wide::buffer_type neo::encoder<neo::utf8, neo::wide>::do_encode(const char* ptr,
                                                                     std::size_t size) {
    std::int32_t errors = 0;
    const auto req_size = ::utf8towide(ptr, size, nullptr, 0, &errors) / sizeof(wchar_t);
    if (req_size == 0 || errors != UTF8_ERR_NONE) {
        throw std::runtime_error("??");  // todo. Probably define an error_category
    }

    return neo::wide::buffer_type::fill(req_size, [&](auto dest) {
        ::utf8towide(ptr, size, dest, req_size * sizeof(wchar_t), nullptr);
    });
}

neo::utf8::buffer_type neo::encoder<neo::wide, neo::utf8>::do_encode(const wchar_t* ptr,
                                                                     std::size_t size) {
    std::int32_t errors = 0;
    const auto req_size = ::widetoutf8(ptr, size * sizeof(wchar_t), nullptr, 0, &errors);
    if (req_size == 0 || errors != UTF8_ERR_NONE) {
        throw std::runtime_error("??");  // todo. Probably define an error_category
    }

    return neo::utf8::buffer_type::fill(req_size, [&](auto dest) {
        ::widetoutf8(ptr, size * sizeof(wchar_t), dest, req_size, nullptr);
    });
}