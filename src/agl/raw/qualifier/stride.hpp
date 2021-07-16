#pragma once

#include "qualifier.hpp"

namespace agl {

template<typename Type>
struct Stride : Qualifier<Type> {
    using Qualifier<Type>::Qualifier;
};

}
