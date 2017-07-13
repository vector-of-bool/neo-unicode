#include <neo/unicode.hpp>

#include <array>
#include <type_traits>

using namespace std;

int main() {
    neo::unicode u = "Meow";
    return u.raw().size();
}