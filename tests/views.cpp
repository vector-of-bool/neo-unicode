#include <neo/unicode/views.hpp>

#include <catch/catch.hpp>

TEST_CASE("Code units for std::string") {
    std::string some_string = "Hello, person!";
    std::wstring wide = L"Hello, wide string!";

    for (auto&& c : neo::code_units(some_string)) {
        CHECK(c != '\0');
    }

    for (auto&& c : neo::code_units(some_string)) {
        CHECK(c != L'\0');
    }

    for (auto c : neo::code_units(std::ref(some_string))) {
        CHECK(c != '\0');
    }

    for (auto c : neo::code_units(std::ref(wide))) {
        CHECK(c != L'\0');
    }

    for (auto c : neo::code_units(std::cref(some_string))) {
        CHECK(c != '\0');
    }

    for (auto c : neo::code_units(std::cref(wide))) {
        CHECK(c != L'\0');
    }
}