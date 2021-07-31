#include <iostream>
#include <map>

int main() {
    auto rules = std::map<char, std::string>{
        {'A', "[&FL!A]/////'[&FL!A]///////'[&FL!A]"},
        {'F', "S/////F"},
        {'S', "FL"},
        {'L', " ['''^^{-f+f+f-|-f+f+f}]" }};
    
    auto current = std::string("A");
    auto next = std::string("");

    for(int i = 1; i < 7; ++i) {
        for(auto c : current) {
            auto it = rules.find(c);
            if(it == end(rules)) {
                next.push_back(c);
            } else {
                next += it->second;
            }
        }
        current = std::move(next);
        next.clear();
    }

    std::cout << current;
}
