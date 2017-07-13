#ifndef NEO_UNICODE_VIEWS_HPP_INCLUDED
#define NEO_UNICODE_VIEWS_HPP_INCLUDED

#include <functional>
#include <type_traits>
#include <utility>


#include "unicode.hpp"

namespace neo {

namespace detail {

template <typename T> struct void_t_helper { using type = void; };

template <typename T> using void_t = typename void_t_helper<T>::type;

template <typename T> struct super_decay { using type = T; };
template <typename T> struct super_decay<std::reference_wrapper<T>> : super_decay<T> {};
template <typename T> struct super_decay<T&> : super_decay<T> {};
template <typename T> struct super_decay<const T> : super_decay<T> {};
template <typename T> struct super_decay<T&&> : super_decay<T> {};
template <typename T> using super_decay_t = typename super_decay<T>::type;

template <typename T> T& deref(std::reference_wrapper<T> t) { return t; }

template <typename T> T& deref(T& t) { return t; }

template <typename Stored, typename Unreffed, typename Void = void> class code_units_iterable;

template <typename Stored, typename Container>
class code_units_iterable<Stored, Container, void_t<typename Container::iterator>> {
    Stored _str;

public:
    code_units_iterable(Stored st)
        : _str(std::move(st)) {}
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using difference_type = typename Container::difference_type;
    using pointer = typename Container::const_pointer;
    using const_pointer = pointer;
    using reference = const value_type&;
    using const_reference = reference;
    using iterator = typename Container::const_iterator;
    using const_iterator = iterator;

    iterator begin() const { return deref(_str).begin(); }
    const_iterator cbegin() const { return deref(_str).cbegin(); }
    iterator end() const { return deref(_str).end(); }
    const_iterator cend() const { return deref(_str).cend(); }
};
}

class code_units_t {
    neo::unicode _str;

public:
    using value_type = char;
    using pointer = const char*;
    using const_pointer = pointer;
    using reference = char&;
    using const_reference = reference;
    using iterator = pointer;
    using const_iterator = iterator;

    // Called with one argument, it produces an iterable
    template <typename What> auto operator()(What&& what) {
        return detail::code_units_iterable<What, detail::super_decay_t<What>>(std::forward<What>(what));
    }
};

class characters_t {};
class text_elements_t {};

namespace {

code_units_t code_units;
characters_t characters;
text_elements_t text_elements;
}
}

#endif  // NEO_UNICODE_VIEWS_HPP_INCLUDED