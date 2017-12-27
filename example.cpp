#include <neo/unicode.hpp>
#include <iostream>

int main() {
    neo::unicode u = "Meow";
    std::wcout << u << '\n';
    // std::cout << u << '\n';
    return 0;
}