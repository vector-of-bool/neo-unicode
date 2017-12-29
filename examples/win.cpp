#include <neo/unicode.hpp>

#include <iostream>

#include <windows.h>

#ifdef _UNICODE
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
    if (argc == 1) {
        return 1;
    }
    neo::unicode filename = "MyFile.txt";
    auto&& buf = filename.encode<neo::native_encoding>();
    auto hnd = ::CreateFile(buf.data(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    neo::unicode my_string = argv[1];
    auto&& utf16_str = my_string.encode<neo::utf16>();
    auto did_write = ::WriteFile(hnd, utf16_str.data(), utf16_str.byte_size(), nullptr, nullptr);
    ::CloseHandle(hnd);
    return did_write ? 0 : 1;
}
