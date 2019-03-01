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
private:
    std::vector<Face> faces_;
    std::vector<Point3> vertices_;
    std::map<Point3, size_t> vertexIndicesMap_;
    size_t vertexCounter_;

public:

    static constexpr size_t NUMBER_OF_SIDES = 3;

    TriMesh() {}
 
    size_t n_Faces() const { return faces_.size(); }

    size_t n_Vertices() const { return vertices_.size(); }

    size_t addVertex(const Point3& p) {
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

    void addFaces(const auto&& faces) { faces_ = faces; }

    const auto& getVertices() const { return vertices_; }

    const Point3& getVertex(size_t idx) const { return vertices_.at(idx); }

    const auto& getFaces() const { return faces_; }

    const Face& getFace(size_t idx) const { return faces_.at(idx); }

    int findOppositeVertexIndex(size_t face_idx, size_t adj_face_idx)
    {
        if (face_idx > n_Faces() || adj_face_idx > n_Faces())
            return -1;

        const Face& face = getFace(face_idx);
        const Face& adj_face = getFace(adj_face_idx);
        std::array<size_t, NUMBER_OF_SIDES> face_vts, adj_face_vts;
        std::array<int, NUMBER_OF_SIDES> diff{-1,};
        boost::range::copy(face.getVertices(), face_vts.begin());
        boost::range::copy(adj_face.getVertices(), adj_face_vts.begin());
        boost::sort(face_vts);
        boost::sort(adj_face_vts);
        auto end = boost::range::set_difference(face_vts, adj_face_vts, diff.begin());
        if (std::distance(diff.begin(), end) > 1) { // more than just one index
            return -1;
        }

        return diff[0];
    }

};

