#include <neo/unicode.hpp>
#include <iostream>

int main() {
    neo::unicode u = "Meow";
    // Automatically output in the stream's encoding
    std::wcout << u << '\n';
    return 0;
}