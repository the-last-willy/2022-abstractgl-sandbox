#include "agl/all.hpp"

using namespace agl;

int main() {
    auto b = Buffer(4);
    agl::bind_buffer(BufferTarget::array, b);
}
