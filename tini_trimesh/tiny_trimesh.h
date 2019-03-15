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

#pragma once

#include <algorithm>
#include <boost/range/algorithm.hpp>
#include <iostream>
#include <map>
#include <array>
#include <vector>
#include <iostream>

struct Point3
{
    double x,y,z;

    bool operator<(const Point3& other) const
    {
        if (x < other.x)
            return true;
        else if (y < other.y)
            return true;
        else
            return z < other.z;
    }
};

class Face
{
public:
    static constexpr size_t NUMBER_OF_VERTICES = 3;
    using VertexList = std::array<size_t, NUMBER_OF_VERTICES>;

    // a constructor
    Face(const VertexList& vlist)
    : vtx_(vlist) {}
  
    const VertexList& getVertices() const { return vtx_; }

private:
    VertexList vtx_;
};

class TriMesh
{
public:
    static constexpr size_t NUMBER_OF_SIDES = 3;

    using FaceIndexList = std::array<int, NUMBER_OF_SIDES>;
    using VertexList = std::vector<Point3>;
    using FaceList = std::vector<Face>;

private:
    VertexList vertices_;
    FaceList faces_;
    std::map<Point3, size_t> vertexIndicesMap_;
    size_t vertexCounter_;

    template <typename ContA, typename ContB>
    std::pair<FaceIndexList,size_t> diffIndices(const ContA& contA, const ContB& contB)
    {
        using boost::copy, boost::sort, boost::range::set_difference;
        FaceIndexList contAcopy, contBcopy, result{-1};
        copy(contA, contAcopy.begin());
        copy(contB, contBcopy.begin());
        sort(contAcopy);
        sort(contBcopy);
        auto pos = set_difference(contAcopy, contBcopy, result.begin());
        size_t dist = std::distance(result.begin(), pos);
        return std::make_pair(std::move(result), dist);
    }

public:

    TriMesh() : vertexCounter_(0) {}
 
    size_t n_Faces() const { return faces_.size(); }

    size_t n_Vertices() const { return vertices_.size(); }

    size_t addVertex(const Point3& p)
    {
        auto it = vertexIndicesMap_.find(p);
        if (it == vertexIndicesMap_.end()) // not found
        {
            vertexIndicesMap_.insert({p, vertexCounter_++});
            vertices_.push_back(std::move(p));
            return vertexCounter_ - 1;
        } else {
            return it->second;
        }
    }

    template <template<typename...> class Cont, typename Vertex, typename... Rest>
    void addVertices(Cont<Vertex, Rest...>&& vertices)
    {
        for (const auto& v : vertices) {
            addVertex(v);
        }
    }

    void addFaces(FaceList&& faces) { faces_ = faces; }

    const auto& getVertices() const { return vertices_; }

    const Point3& getVertex(size_t idx) const { return vertices_.at(idx); }

    const auto& getFaces() const { return faces_; }

    const Face& getFace(size_t idx) const { return faces_.at(idx); }

    std::pair<Point3,Point3> getXYplaneBB(const TriMesh& mesh)
    {
        using boost::transform;
        const auto& vertexList = mesh.getVertices();
        std::vector<double> x_coord, y_coord;
        transform(vertexList, std::back_inserter(x_coord), [](const Point3& p) -> double { return p.x; });
        transform(vertexList, std::back_inserter(y_coord), [](const Point3& p) -> double { return p.y; });
        auto x_minmax = std::minmax_element(x_coord.begin(), x_coord.end());
        auto y_minmax = std::minmax_element(y_coord.begin(), y_coord.end());
        return std::pair<Point3,Point3>({*x_minmax.first,  *y_minmax.first,  0.0},
                                        {*x_minmax.second, *y_minmax.second, 0.0});
    }

    int findOppositeVertexIndex(size_t face_idx, size_t adj_face_idx)
    {
        if (face_idx > n_Faces() || adj_face_idx > n_Faces())
            return -1;

        const Face& face = getFace(face_idx);
        const Face& adj_face = getFace(adj_face_idx);
        auto [diff,dist] = diffIndices(face.getVertices(), adj_face.getVertices());
        if (dist > 1) { // more than just one index
            return -1;
        }

        return diff[0];
    }

};
