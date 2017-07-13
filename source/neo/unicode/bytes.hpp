#ifndef NEO_BYTES_HPP_INCLUDED
#define NEO_BYTES_HPP_INCLUDED

#include <string>

namespace neo {

class bytes {
    std::string _data;

public:
    using size_type = std::string::size_type;
    using difference_type = std::string::difference_type;
    using value_type = std::string::value_type;
    using reference = std::string::reference;
    using const_reference = std::string::const_reference;
    using pointer = std::string::pointer;
    using const_pointer = std::string::const_pointer;
    using iterator = std::string::iterator;
    using const_iterator = std::string::const_iterator;
    using reverse_iterator = std::string::reverse_iterator;
    using const_reverse_iterator = std::string::const_reverse_iterator;

    bytes() = default;
    bytes(const bytes&) = default;
    bytes(bytes&&) = default;
    bytes& operator=(const bytes&) = default;
    bytes& operator=(bytes&&) = default;

    template <typename Iter>
    bytes(Iter first, Iter last) : _data(first, last) {}

    template <typename Iter>
    void assign(Iter first, Iter last) { return _data.assign(first, last); }

    size_type size() const { return _data.size(); }
    iterator begin() { return _data.begin(); }
    iterator end() { return _data.end(); }
    const_iterator begin() const { return _data.begin(); }
    const_iterator end() const { return _data.end(); }
    const_iterator cbegin() const  { return begin(); }
    const_iterator cend() const { return end(); }

    reverse_iterator rbegin() { return _data.rbegin(); }
    reverse_iterator rend() { return _data.rend(); }
    const_reverse_iterator rbegin() const { return _data.rbegin(); }
    const_reverse_iterator rend() const { return _data.rend(); }
    const_reverse_iterator crbegin() const  { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

    size_type capacity() const { return _data.capacity(); }
    void resize(size_type new_size) { return _data.resize(new_size); }
    void push_back(value_type c) { _data.push_back(c); }

    pointer data() { return size() == 0 ? nullptr : &_data[0]; }
    const_pointer data() const { return size() == 0 ? nullptr : &_data[0]; }
};

}

#ifdef NEO_DISABLED_MSVC_SECURE_WARNINGS
#undef NEO_DISABLED_MSVC_SECURE_WARNINGS
#undef _SCL_SECURE_NO_WARNINGS
#endif

#endif // NEO_BYTES_HPP_INCLUDED