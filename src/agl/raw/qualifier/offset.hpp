#pragma once

#include "qualifier.hpp"

namespace agl {

template<typename Type>
struct Offset : Qualifier<Type> {
    using Qualifier<Type>::Qualifier;
};

}
