#pragma once
#include "face.hpp"
#include "vertex.hpp"
#include "triangle_mesh/all.hpp"

#include <vector>
#include <map>

namespace chaine {

struct FaceVertexMesh {
    std::vector<Vertex> vertices = {};
    std::vector<Face> faces = {};
};

struct FacePair {
    unsigned vertex;
    unsigned face; 
};

FaceVertexMesh to_face_vertex_mesh(TriangleMesh mesh) {

    FaceVertexMesh fvmesh;

    std::map<std::pair<unsigned, unsigned>, FacePair> m;

    for(auto i = 0;  i < mesh.vertex_positions.size(); ++i) {
        fvmesh.vertices[i].point = mesh.vertex_positions[i];
    }

    for(auto i = 0; i < mesh.triangle_indices.size(); ++i) {
        auto indices = fvmesh.faces[i].vertices;
        indices = mesh.triangle_indices[i];
        fvmesh.vertices[indices[0]].index = i;

        auto it = m.find(std::pair(indices[0], indices[1]));
        if (it != m.end()) {
            auto fp = it->second;
        }
    }

    return fvmesh;
}
}