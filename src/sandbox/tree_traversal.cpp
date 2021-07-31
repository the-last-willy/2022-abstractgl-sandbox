#include "turtle/all.hpp"

using namespace turtle;

int main() {
    auto t1 = Tree{
        .model_id = 3,
        .children = {}
    };
    auto t2 = Tree{
        .model_id = 2,
        .children = { t1 }
    };
    auto t = Tree{
        .model_id = 0,
        .children = {
            Tree{
                .model_id = 1,
                .children = {
                    
                }
            },
            t2
        }};

    traverse(t, TreePrinter());
}
