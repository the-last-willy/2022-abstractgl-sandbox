#pragma once

#include "qualifier.hpp"

namespace agl {

template<typename Type>
struct BindingIndex : Qualifier<Type> {
    using Qualifier<Type>::Qualifier;
};

}
