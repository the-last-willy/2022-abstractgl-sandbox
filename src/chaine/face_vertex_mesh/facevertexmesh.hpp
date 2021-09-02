#pragma once
#include "Face.hpp"
#include "Vertex.hpp"
#include <vector>

struct FaceVertexMesh {
    std::vector<Vertex> vertices = {};
    std::vector<Face> faces = {};
};