#pragma once

#include "qualifier.hpp"

namespace agl {

template<typename Type>
struct InstanceCount : Qualifier<Type> {
    using Qualifier<Type>::Qualifier;
};

}
