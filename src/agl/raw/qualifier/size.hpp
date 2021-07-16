#pragma once

#include "qualifier.hpp"

namespace agl {

template<typename Type>
struct Size : Qualifier<Type> {
    using Qualifier<Type>::Qualifier;
};

}
