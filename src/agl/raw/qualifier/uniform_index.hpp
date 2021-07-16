#pragma once

#include "qualifier.hpp"

namespace agl {

template<typename Type>
struct UniformIndex : Qualifier<Type> {
    using Qualifier<Type>::Qualifier;
};

}
