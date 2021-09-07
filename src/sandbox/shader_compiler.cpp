#include "engine/shader.hpp"
#include "local/all.hpp"
 
#include "root.hpp"

#include <iostream>

void throwing_main() {
    auto sc = eng::ShaderCompiler();
    sc.root = local::src_folder + "src/shader/";

    std::cout << sc("gltf/skybox.fs", agl::fragment_shader_tag) << std::endl;
}

int main() {
    try {
        throwing_main();
        return 0;
    } catch(const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
    }
    return -1;
}
