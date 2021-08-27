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

inline
float end_time(const AnimationSampler& as) {
    if(as.input.component_type == GL_FLOAT) {
        return max<float>(as.input).value();
    } else {
        throw std::runtime_error("Wrong accessor min type.");
    }
}

struct AnimationChannel {
    std::shared_ptr<Node> target_node = {};
    AnimationTargetPath target_path = {};

    std::shared_ptr<AnimationSampler> sampler = {};
};

struct Animation {
    // SHARED PTR USELESS HERE. TODO: DO.
    std::vector<std::shared_ptr<AnimationChannel>> channels = {};
};

inline
float end_time(const Animation& a) {
    auto time = 0.f;
    for(auto& c : a.channels) {
        auto c_end_time = end_time(*c->sampler);
        if(time < c_end_time) {
            time = c_end_time;
        }
    }
    return time;
}

struct AnimationInterpolation {
    std::size_t previous = {};
    std::size_t next = {};
    float t = 0.f;
};

struct AnimationPlayer {
    std::shared_ptr<Animation> animation = {};

    float time = 0.f; // In seconds.

    AnimationInterpolation index(std::size_t channel_i) const {
        auto& timeline = animation->channels[channel_i]->sampler->input;
        auto ai = AnimationInterpolation();
        if(timeline.count) {
            auto first_keyframe = at<float>(timeline, 0);
            if(first_keyframe > time) {
                ai.previous = 0;
                ai.next = 0;
                return ai;
            }
            for(std::size_t i = 1; i < timeline.count; ++i) {
                auto previous_keyframe = at<float>(timeline, i - 1);
                auto next_keyframe = at<float>(timeline, i);
                if(next_keyframe > time) {
                    ai.previous = i - 1;
                    ai.next = i;
                    ai.t = (time - previous_keyframe) / (next_keyframe - previous_keyframe);
                    return ai;
                }
            }
            ai.previous = timeline.count - 1;
            ai.next = timeline.count - 1;
            return ai;
        }
        throw std::runtime_error("Animation channel without a single keyframe.");
    }   

    void update(float dt) {
        time += dt;
        auto et = end_time(*animation);
        if(time >= et) {
            time -= et;
        }
    }
};

}
