#pragma once

#include <filesystem>
#include <stdexcept>

namespace filesystem {

inline
std::filesystem::path
recursive_parent_path(
    std::filesystem::path base,
    std::filesystem::path target)
{
    while(not base.empty()) {
        auto full_path = base / target;
        if(exists(full_path)) {
            return full_path;
        }
        base = base.parent_path();
    }
    throw std::runtime_error(
        "Failed to open \"" + target.string() + "\".");
}

inline
std::filesystem::path
recursive_parent_path(
    std::filesystem::path target)
{
    return recursive_parent_path(
        std::filesystem::current_path(),
        target);
}

}
