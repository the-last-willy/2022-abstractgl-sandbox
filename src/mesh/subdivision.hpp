#pragma once

#include "face_vertex.hpp"

namespace mesh {

// Currently no seam handling.
// So it completely destroys connectivity.
template<typename Vertex>
auto subdivided(const FaceVertex<Vertex>& face_vertex, unsigned n) {
    auto result = FaceVertex<Vertex>();
    { // Triangles.
        {
            auto factor = (n + 1) * (n + 1);
            result.triangles.reserve(factor * size(face_vertex.triangles));
        }
        for(int t = 0; t < size(face_vertex.triangles); ++t) {
            auto offset = (n * (n + 5) / 2 + 3) * t;
            auto i_shift = [n](auto x) {
                return x * (2 * n - x + 5) / 2;
            };
            // External triangles.
            for(unsigned i = 0; i < n + 1; ++i)
            for(unsigned j = 0; j < n - i + 1; ++j) {
                result.triangles.push_back(Triangle{
                    offset + i_shift(i + 0) + j + 0,
                    offset + i_shift(i + 1) + j + 0,
                    offset + i_shift(i + 0) + j + 1,});
            }
            // Internal triangles.
            for(unsigned i = 0; i < n; ++i)
            for(unsigned j = 1; j < n - i + 1; ++j) {
                result.triangles.push_back(Triangle{
                    offset + i_shift(i + 0) + j + 0,
                    offset + i_shift(i + 1) + j - 1,
                    offset + i_shift(i + 1) + j + 0,});
            }
        }
    }
    { // Vertices.
        {
            auto factor = n * (n + 5) / 2 + 3;
            result.vertices.reserve(factor * size(face_vertex.triangles));
        }
        for(int t = 0; t < size(face_vertex.triangles); ++t) {
            auto offset = 3 * t;
            auto a = face_vertex.vertices[cube_indexes[offset + 0]];
            auto b = face_vertex.vertices[cube_indexes[offset + 1]];
            auto c = face_vertex.vertices[cube_indexes[offset + 2]];
            for(unsigned i = 0; i < n + 2; ++i)
            for(unsigned j = 0; j < n - i + 2; ++j) {
                auto tb = float(i) / (n + 1);
                auto tc = float(j) / (n + 1);
                result.vertices.push_back(
                    barycentric_interpolation(a, b, c, tb, tc));
            }
        }
    }
    return result;
}

}
