#ifndef NEO_UNICODE_CONCEPTS_HPP_INCLUDED
#define NEO_UNICODE_CONCEPTS_HPP_INCLUDED

#include <type_traits>

#if NEO_UNICODE_ENABLE_CONCEPTS && !defined(neo_concept)
#define neo_concept concept
#define neo_concept_param(T) T
#elif !defined(neo_concept)
#define neo_concept
#define neo_concept_param(T) typename
#endif

namespace neo {

template <typename T> using character_set_type_t = typename T::character_set;
template <typename T> using code_point_type_t = typename T::code_point_type;
template <typename T> using buffer_type_t = typename T::buffer_type;
template <typename T> using value_type_t = typename T::value_type;
template <typename T> using pointer_t = typename T::pointer;
template <typename T> using const_pointer_t = typename T::const_pointer;
template <typename T> using size_type_t = typename T::size_type;


namespace unicode_detail {

template <typename Default, typename Void, template <class...> class Operator, typename... Args>
struct detect_helper {
    using value_t = std::false_type;
    using type = Default;
};

template <typename T> using void_t = void;

template <typename Default, template <class...> class Operator, typename... Args>
struct detect_helper<Default, void_t<Operator<Args...>>, Operator, Args...> {
    using value_t = std::true_type;
    using type = Operator<Args...>;
};

struct nonesuch {};

template <template <class...> class Operator, typename... Args>
using is_detected = typename detect_helper<nonesuch, void, Operator, Args...>::value_t;

template <template <class...> class Operator, typename... Args>
using detected_t = typename detect_helper<nonesuch, void, Operator, Args...>::type;

template <typename Default, template <class...> class Operator, typename... Args>
using detected_or = typename detect_helper<Default, void, Operator, Args...>::type;

template <template <class...> class Operator, typename... Args>
constexpr bool is_detected_v = is_detected<Operator, Args...>::value;

template <typename T> using get_name_t = decltype(T::get_name());

template <typename T> using equal_compare_t = decltype(std::declval<T&>() == std::declval<T&>());

template <typename Target, typename... Args>
constexpr neo_concept bool Constructible = std::is_constructible<Target, Args...>::value;

template <typename From, typename To>
constexpr neo_concept bool Convertible = std::is_convertible<From, To>::value;

template <typename T> constexpr neo_concept bool CodeUnit = std::is_trivial<T>::value;

template <typename T> using data_t = decltype(std::declval<T&>().data());

template <typename T>
constexpr neo_concept bool CodeUnitBuffer = CodeUnit<typename T::value_type>&&
    Convertible<decltype(*std::declval<typename T::pointer&>()), typename T::value_type>&&
        Convertible<decltype(*std::declval<typename T::const_pointer&>()), typename T::value_type>&&
            Convertible<detected_t<data_t, T>, typename T::const_pointer>;

template <typename T>
constexpr neo_concept bool Encoding
    = Convertible<unicode_detail::detected_t<unicode_detail::get_name_t, T>, const char*>&&
        unicode_detail::is_detected_v<buffer_type_t, T>;

}  // namespace unicode_detail

inline namespace concepts {

template <typename T> constexpr neo_concept bool CodeUnit = unicode_detail::CodeUnit<T>;
template <typename T> constexpr neo_concept bool CodePoint = CodeUnit<T>;
template <typename T> constexpr neo_concept bool CodeUnitBuffer = unicode_detail::CodeUnitBuffer<T>;
template <typename T> constexpr neo_concept bool Encoding = unicode_detail::Encoding<T>;

// template <typename T>
// constexpr neo_concept bool CharacterSet = CodePoint<code_point_type_t<T>>&&
//     Convertible<unicode_detail::detected_t<unicode_detail::get_name_t, T>, const char*>&&
//         Convertible<unicode_detail::detected_t<unicode_detail::get_substitution_code_point_t,
//         T>,
//                     code_point_type_t<T>>;

// template <typename T>
// constexpr neo_concept bool Copyable = std::is_copy_assignable<T>::value&&
//     std::is_copy_assignable<T>::value&& std::is_move_assignable<T>::value;

// template <typename T>
// constexpr neo_concept bool Semiregular = Copyable<T>&& std::is_default_constructible<T>::value;

// template <typename T>
// constexpr neo_concept bool EqualityComparable
//     = Convertible<unicode_detail::detected_t<unicode_detail::equal_compare_t, T>, bool>;

// template <typename T> constexpr neo_concept bool Regular = Semiregular<T>&&
// EqualityComparable<T>;

// template <typename T>
// constexpr neo_concept bool Character = Regular<T>&& CharacterSet<character_set_type_t<T>>&&
//     Constructible<T, code_point_type_t<character_set_type_t<T>>>;


}  // namespace concepts

}  // namespace neo

#endif  // NEO_UNICODE_CONCEPTS_HPP_INCLUDED