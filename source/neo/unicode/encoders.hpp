#ifndef NEO_ENCODERS_HPP_INCLUDED
#define NEO_ENCODERS_HPP_INCLUDED

#include "bytes.hpp"

namespace neo {

class unicode;

struct utf8 {
    template <typename Allocator>
    static unicode decode(const char* first, const char* last);
};

}

#endif // NEO_ENCODERS_HPP_INCLUDED