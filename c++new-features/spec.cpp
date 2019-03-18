#include <iostream>
#include <algorithm>
#include <functional>
#include <array>
#include <vector>

template <typename T>
struct Inserter
{
    using OutputIterator = typename T::iterator;
    OutputIterator operator()(T& cont) { return cont.begin(); }
};

template <typename T, typename... Rest>
struct Inserter<std::vector<T, Rest...>>
{
    using OutputIterator = std::back_insert_iterator<std::vector<T, Rest...>>;
    OutputIterator operator()(std::vector<T>& cont) { return std::back_inserter(cont); }
};

template <typename T>
typename Inserter<T>::OutputIterator append(T& cont)
{
    Inserter<T> inserter;
    return inserter(cont);
}

int main()
{
    std::array<double, 3> arr{1,2,3}, crr{17,18,19};
    std::vector<double> brr{7,8,9};
    std::copy(std::begin(brr), std::end(brr), append(arr));
    std::cout << "arr[0] : " << arr[0] << " arr[1] : " << arr[1] << " arr[2] : " << arr[2] << " arr.size() : " << arr.size() << std::endl;
    std::copy(std::begin(crr), std::end(crr), append(brr));
    std::cout << "brr[3] : " << brr[3] << " brr[1] : " << brr[1] << " brr[2] : " << brr[2] << " brr.size() : " << brr.size() << std::endl;
}

