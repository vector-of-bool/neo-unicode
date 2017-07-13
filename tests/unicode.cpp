#include <neo/unicode.hpp>
#include <neo/unicode/bytes.hpp>

#include <catch/catch.hpp>

using namespace neo;
using namespace neo::literals;

TEST_CASE("Create default") {
    neo::unicode u;
}

TEST_CASE("From char literal") {
    unicode u = "Hello!";
    auto u2 = "Hello!"_u;
}

TEST_CASE("Raw view") {
    unicode u = "Hi";
    auto r = u.raw();
    "Howdy!"_u.raw();
    for (auto b : "HHHHHH"_u.raw()) {
        CHECK(b == 'H');
    }
}

TEST_CASE("Copying") {
    unicode u = "Hello!";
    unicode copy = u;
    CHECK(copy.raw().size() == 6);
}

TEST_CASE("From std::string") {
    std::string str = "Hello, unicode!";
    unicode u = str;
    CHECK(u.raw().size() == str.size());
}

inline void meow(unicode u) {
    std::puts(u.raw().begin());
}

TEST_CASE("Encoding") {
    auto u = "This is a unicode string!"_u;

    auto bytes = u.encode(utf16);
    CHECK(bytes.size() == 50);

    u = "This is a unicode string!";
    // meow(u);
    bytes = u.encode(utf16);
    CHECK(bytes.size() == 50);
}