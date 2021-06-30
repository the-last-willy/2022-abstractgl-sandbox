#pragma once

#include <fstream>
#include <stdexcept>

namespace tlw {

inline
std::string file(const std::string& filepath) {
    auto ifs = std::fstream(filepath);
    if(ifs.fail()) {
        throw std::runtime_error(
            "Failed to openg file \"" + filepath + "\".");
    }
    return std::string(
        std::istreambuf_iterator<char>(ifs),
        std::istreambuf_iterator<char>());
}

}
