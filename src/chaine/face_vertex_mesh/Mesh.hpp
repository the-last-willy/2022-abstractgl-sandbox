#pragma once
#include "Face.hpp"
#include "Vertex.hpp"
#include <vector>

struct Mesh {
    std::vector<Vertex> vertices = {};
    std::vector<Face> faces = {};
};