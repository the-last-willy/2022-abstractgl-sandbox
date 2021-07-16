#pragma once

#include "qualifier.hpp"

namespace agl {

template<typename Type>
struct Count : Qualifier<Type> {
    using Qualifier<Type>::Qualifier;
};

}
