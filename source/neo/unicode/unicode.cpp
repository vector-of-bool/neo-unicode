#include "unicode.hpp"

#include "bytes.hpp"

#include <utf8rewind.h>

neo::utf8_t neo::utf8;
neo::utf16_t neo::utf16;

using namespace neo;

bytes utf8_t::encode(const unicode& u) {
    const auto raw_view = u.raw();
    return bytes(raw_view.begin(), raw_view.end());
}

bytes utf16_t::encode(const unicode& e) {
    auto utf8_bytes = e.encode(utf8);
    bytes utf16_bytes;
    utf16_bytes.resize(utf8_bytes.size() * 2 + 32);
    while (1) {
        const auto get_outptr = [&] { return reinterpret_cast<::utf16_t*>(utf16_bytes.data()); };
        std::int32_t errors = 0;
        const auto real_size = ::utf8toutf16(utf8_bytes.data(), utf8_bytes.size(), get_outptr(), utf16_bytes.size(), &errors);
        switch (errors) {
        case UTF8_ERR_NONE: {
            utf16_bytes.resize(real_size);
            return utf16_bytes;
        }
        case UTF8_ERR_INVALID_DATA:
            throw std::runtime_error("TODO: A real exception");
        case UTF8_ERR_NOT_ENOUGH_SPACE: {
            utf16_bytes.resize(real_size);
            continue;
        default:
            std::terminate();
        }
        }
    }
}