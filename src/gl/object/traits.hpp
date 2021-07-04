#pragma once

#include <glad/glad.h>

#include <concepts>
#include <type_traits>

namespace tlw {
namespace gl {

template<typename Traits>
concept ObjectTraits = requires {
    { Traits::create() } -> std::same_as<GLuint>;
    { Traits::delete_(std::declval<GLuint>()) };
    { Traits::is(std::declval<GLuint>()) } -> std::convertible_to<GLboolean>; 
};

}}
