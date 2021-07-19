#include "agl/all.hpp"

using namespace agl;

int main() {
    auto b = Buffer();
    glBufferStorage(0, 0, 0, 0);
    agl::bind_buffer(BufferTarget::array, b);
}
