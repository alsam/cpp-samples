#include <iostream>
#include <vector>

template <template<typename,typename> class Cont, typename Elem, typename Alloc>
std::ostream& operator<<(std::ostream& stream, Cont<Elem,Alloc> const& cont)
{
    stream << "{";
    for (auto const& c : cont) {
        stream << c << ", ";
    }
    stream << "}" << std::endl;
    return stream;
}

void sub(std::vector<int> const& vec_)
{
    std::vector<int> vec(vec_);
    vec.push_back(42);
    std::cout << "vec: " << vec << std::endl;
}

void sub(std::vector<int> && vec)
{
    vec.push_back(42);
    std::cout << "vec: " << vec << std::endl;
}


int main()
{
    std::vector<int> v = {1,2,3};
    std::cout << "v: " << v << std::endl;
    sub(v);
    std::cout << "v: " << v << std::endl;
    sub({1,2,3});

}
