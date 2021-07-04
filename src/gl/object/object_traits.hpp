#pragma once

#include <glad/glad.h>

#include <utility>

namespace tlw {
namespace gl {

template<typename Traits>
concept ObjectTraits = requires {
    { Traits::create(); } -> std::same<GLuint>;
    { Traits::delete(std::declval<GLuint>()); };
    { Traits::is(std::declval<GLuint>()); } -> std::same<GLboolean>; 
};

}}
