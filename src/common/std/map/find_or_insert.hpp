#pragma once

#include <map>

namespace common {

template<typename K, typename V, typename C, typename A, typename Key, typename Value>
auto& find_or_insert(
    std::map<K, V, C, A>& m, Key&& k, Value&& default_)
{
    auto it = m.find(k);
    if(it == end(m)) {
        return m[k] = std::move(default_);
    } else {
        return it->second;
    }
}


}
