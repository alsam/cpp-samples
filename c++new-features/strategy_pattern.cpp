
// borrowed and modified from here [Effective GoF Patterns with C++11 and Boost Tobias Darm](https://accu.org/content/conf2013/Tobias_Darm_Effective_GoF_Patterns.pdf)

#include <functional>
#include <vector>
#include <iostream>

class CaffeineBeverage
{
public:
    CaffeineBeverage(std::function<int()> amount_water_ml, std::function<void()> brew)
    : amount_water_ml_(amount_water_ml)
    , brew_(brew)
    {}

    void prepare() {
        boil_water_(amount_water_ml_());
        brew_();
    }

private:

    void boil_water_(int) {}

    std::function<int()> amount_water_ml_;
    std::function<void()> brew_;
};

class Recipes
{
public:

    static void brew_coffee() { std::cout << "brew coffee\n";}

    static void brew_tea() { std::cout << "brew tea\n";}

    static int amount_water_ml(int ml) { return ml; }
};

using Beverages = std::vector<CaffeineBeverage*>;

int main()
{
    CaffeineBeverage coffee([]{ return Recipes::amount_water_ml(150); }, &Recipes::brew_coffee );
    CaffeineBeverage tea([]{ return Recipes::amount_water_ml(200); }, &Recipes::brew_tea );
    Beverages beverages = { &coffee, &tea };

    for (auto beverage : beverages) {
        beverage->prepare();
    }
}
