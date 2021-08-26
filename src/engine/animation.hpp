#pragma once

#include "accessor.hpp"
#include "node.hpp"

#include <vector>

namespace eng {

enum class AnimationSamplerInterpolation {
    cubic_spline,
    linear,
    step
};

enum class AnimationTargetPath {
    rotation,
    scale,
    translation,
    weights
};

struct AnimationSampler {
    eng::Accessor input = {};
    eng::Accessor output = {};

    AnimationSamplerInterpolation interpolation = {};
};

struct AnimationChannel {
    std::shared_ptr<Node> target_node = {};
    AnimationTargetPath target_path = {};

    std::shared_ptr<AnimationSampler> sampler = {};
};

struct Animation {
    // SHARED PTR USELESS HERE. TODO: DO.
    std::vector<std::shared_ptr<AnimationChannel>> channels = {};
};

struct AnimationInterpolation {
    std::size_t a = {};
    std::size_t b = {};
    float t = {};
};

struct AnimationPlayer {
    std::shared_ptr<Animation> animation = {};

    float time = 0.f; // In seconds.

    int index(std::size_t channel_i) const {
        auto& timeline = animation->channels[channel_i]->sampler->input;
        auto ai = AnimationInterpolation();
        for(std::size_t i = 0; i < timeline.count; ++i) {
            auto frame_time = at<float>(timeline, i);
            if(frame_time > time) {
                return i - 1;
            }
        }
        return timeline.count - 1;
    }   

    void update(float dt) {
        time += dt;
    }
};

}
