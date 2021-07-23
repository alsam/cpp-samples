// clientTemplate.cpp

#include <iostream>
#include <string>
import math;

int main() {
    
    std::cout << std::endl;     

    std::cout << "math::sum(2000, 11): " << math::sum(2000, 11) << std::endl;

    std::cout << "math::sum(2013.5, 0.5): " << math::sum(2013.5, 0.5) << std::endl;
 
    std::cout << "math::sum(2017, false): " << math::sum(2017, false) << std::endl;

    std::cout << "math::sum(\"ab\", \"cd\"): " << math::sum(std::string("ab"), std::string("cd")) << std::endl;
    
}
