#include <iostream>
#include <map>

struct TurtleState {

};

struct Turtle {

};

int main() {
    auto rules = std::map<char, std::string>{
        {'F', "FF"},
        {'X', "F+[[X]-X]-F[-FX]+X"},};
    
    auto current = std::string("X");
    auto next = std::string("");

    for(int i = 0; i < 3; ++i) {
        for(auto c : current) {
            auto it = rules.find(c);
            if(it == end(rules)) {
                next.push_back(c);
            } else {
                next += it->second;
            }
        }
        std::cout << next << std::endl;
        current = std::move(next);
        next.clear();
    }

    
}
