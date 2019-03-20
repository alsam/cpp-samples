// The MIT License (MIT)
//
// Copyright (c) 2019 Alexander Samoilov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

#include <iostream>
#include <algorithm>
#include <functional>
#include <array>
#include <vector>
#include <set>
#include <functional>
#include <boost/range/algorithm.hpp>

template <typename T>
struct Inserter
{
    using Type = T;
    using OutputIterator = typename T::iterator;
    OutputIterator operator()(Type& cont) { return cont.begin(); }
    size_t dist(const Type& cont, OutputIterator it)
    {
        return std::distance<typename T::const_iterator>(cont.begin(), it);
    }
};

template <typename T, typename... Rest>
struct Inserter<std::vector<T, Rest...>>
{
    using Type = std::vector<T, Rest...>;
    using OutputIterator = std::back_insert_iterator<Type>;
    OutputIterator operator()(Type& cont) { return std::back_inserter(cont); }
    size_t dist(const Type& cont, OutputIterator)
    {
        return cont.size();
    }
};

template <typename T>
typename Inserter<T>::OutputIterator append(T& cont)
{
    Inserter<T> inserter;
    return inserter(cont);
}

template <typename T>
size_t dist(const T& cont, typename Inserter<T>::OutputIterator it)
{
    return Inserter<T>().dist(cont, it);
}

template <typename RetCont, typename AIterator, typename BIterator, typename SetOp>
int setOpIndices(RetCont& result,
                 AIterator contAbegin, AIterator contAend,
                 BIterator contBbegin, BIterator contBend,
                 SetOp setOp)
{
    RetCont contAcopy, contBcopy;
    std::copy(contAbegin, contAend, append(contAcopy));
    std::copy(contBbegin, contBend, append(contBcopy));
    boost::sort(contAcopy);
    boost::sort(contBcopy);
    auto initialPosition = append(result);
    auto pos = setOp(contAcopy, contBcopy, append(result));
    return dist(result, pos);
}

template <typename RetCont, typename AIterator, typename BIterator,
          typename OutputIterator = typename RetCont::iterator>
int diffIndices(RetCont& result,
                AIterator contAbegin, AIterator contAend,
                BIterator contBbegin, BIterator contBend)
{
    return setOpIndices(result, contAbegin, contAend, contBbegin, contBend,
      boost::set_difference<RetCont, RetCont, OutputIterator>);
}

template <typename RetCont, typename AIterator, typename BIterator,
          typename OutputIterator = typename RetCont::iterator>
int intersectIndices(RetCont& result,
                     AIterator contAbegin, AIterator contAend,
                     BIterator contBbegin, BIterator contBend)
{
    return setOpIndices(result, contAbegin, contAend, contBbegin, contBend,
      boost::set_intersection<RetCont, RetCont, OutputIterator>);
}

// range-based syntax sugar BEWARE uses std::back_inserter 
template <typename RetCont, typename ARange, typename BRange>
int diffIndicesRange(RetCont& result, const ARange& acont, const BRange& bcont)
{
    result.clear();
    return diffIndices<RetCont, typename ARange::const_iterator, typename BRange::const_iterator,
                       std::back_insert_iterator<RetCont>>
      (result, begin(acont), end(acont), begin(bcont), end(bcont));
}

template <typename RetCont, typename ARange, typename BRange>
int intersectIndicesRange(RetCont& result, const ARange& acont, const BRange& bcont)
{
    result.clear();
    return intersectIndices<RetCont, typename ARange::const_iterator, typename BRange::const_iterator,
                            std::back_insert_iterator<RetCont>>
      (result, begin(acont), end(acont), begin(bcont), end(bcont));
}

#include <gtest/gtest.h>

TEST(SetOps, FixedSizeContainer)
{
    std::array<int, 3> a{1, 2, 3}, b{3, 4, 5}, c;
    size_t size = diffIndices(c, begin(a), end(a), begin(b), end(b));
    ASSERT_EQ(size, 2);
    ASSERT_EQ(c[0], 1);
    ASSERT_EQ(c[1], 2);
    size = intersectIndices(c, begin(a), end(a), begin(b), end(b));
    ASSERT_EQ(size, 1);
    ASSERT_EQ(c[0], 3);
}

TEST(SetOps, VariableSizeContainer)
{
    std::vector<int> a{1, 2, 3}, b{3, 4, 5}, c;
    size_t size = diffIndicesRange(c, a, b);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(c[0], 1);
    ASSERT_EQ(c[1], 2);
    size = intersectIndicesRange(c, a, b);
    ASSERT_EQ(size, 1);
    ASSERT_EQ(c[0], 3);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
