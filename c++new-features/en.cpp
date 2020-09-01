#include <vector>
#include <iostream>
#include "range.h"

struct vec3 {
    double x,y,z;
};

int main() {
    std::vector<vec3> points = {{6,5,8},{1,2,3},{7,3,7}};
    std::vector<double> weights = {4,6,9};

    for (auto [i, p]: enumerate(points)) {
        std::cout << i << " " << p.x << " " << weights[i] << '\n';
    }

    return 0;
}
