#pragma once

#include "qualifier.hpp"

namespace agl {

template<typename Type>
struct AttributeIndex : Qualifier<Type> {
    using Qualifier<Type>::Qualifier;
};

}
