#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <gtest/gtest.h>

// [Getting error "BOOST_PARAMETER_MAX_ARITY must be at least 12 for CGAL::Mesh_3" in CGAL 4.9](http://cgal-discuss.949826.n4.nabble.com/Getting-error-quot-BOOST-PARAMETER-MAX-ARITY-must-be-at-least-12-for-CGAL-Mesh-3-quot-in-CGAL-4-9-td4662304.html)
#define BOOST_PARAMETER_MAX_ARITY 12

#include <CGAL/Polyhedron_3.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/iterator/zip_iterator.hpp>

using Kernel = CGAL::Simple_cartesian<double>;
using Aff_transformation_3 = CGAL::Aff_transformation_3<Kernel>;
using Polyhedron = CGAL::Polyhedron_3<Kernel>;
using Color = CGAL::Color;
using HalfedgeDS = Polyhedron::HalfedgeDS;
using HDS = Polyhedron::Halfedge_data_structure;
using Decorator = CGAL::HalfedgeDS_decorator<HDS>;

using Items = std::map<std::string, Polyhedron>;

class SyntheticSeamCollinearTest : public testing::Test
{
protected:

  Items getItems(const std::string &file_name)
  {
    std::string test_models_path = pathFromUri("package://analyzer/resources/");
    auto items = parseScene(test_models_path, test_models_path + file_name);
    return items;
  }

  SearchTree makeSearchTree(const std::vector<LineSet>& weld_segments)
  {
    std::vector<Point> points;
    std::vector<size_t> seg_indices;
    for (size_t i = 0; i < weld_segments.size(); ++i)
    {
      const auto &seg = weld_segments[i];
      Point p1 = seg.front();
      Point p2 = seg.back();
      points.push_back(std::move(p1));
      points.push_back(std::move(p2));
      seg_indices.push_back(i);
      seg_indices.push_back(i);
    }

    return SearchTree(
      boost::make_zip_iterator(boost::make_tuple( points.begin(), seg_indices.begin() )),
      boost::make_zip_iterator(boost::make_tuple( points.end(),   seg_indices.end() ) )
    );
  }

};

class MyTriMeshRefiner
{
private:
  std::vector<std::vector<size_t>> faces_;
  std::vector<Point3> vertices_;
  std::map<Point3, size_t> vertexIndicesMap_;
  size_t vertexCounter_;

public:

  size_t n_Faces() const { return faces_.size(); }

  size_t n_Vertices() const { return vertices_.size(); }

  size_t addVertex(Point3 p)
  {
    if (auto it = vertexIndicesMap_.find(p); it == vertexIndicesMap_.end()) // not found
    {
      vertexIndicesMap_.insert({p, vertexCounter_++});
      vertices_.push_back(std::move(p));
      return vertexCounter_ - 1;
    }
    else
    {
      return it->second;
    }
  }

  MyTriMeshRefiner(const Polyhedron& poly)
  : vertexCounter_(0)
  {
    for (const auto &face : faces(poly))
    {
      auto begin = face->halfedge();
      auto edge = begin;
      // Each vertex
      std::vector<size_t> tri;
      do
      {
        Point3 p = edge->vertex()->point();
        size_t vertex_idx = addVertex(p);
        tri.push_back(vertex_idx);
        edge = edge->next();
      } while (edge != begin);
      faces_.push_back(std::move(tri));
    }
  }

  void refine()
  {
    std::vector<std::vector<size_t>> new_faces;
    for (const auto &face : faces_)
    {
      size_t a_idx = face[0],
             b_idx = face[1],
             c_idx = face[2];
      Point3 a = vertices_[a_idx],
                  b = vertices_[b_idx],
                  c = vertices_[c_idx],
                  ab = CGAL::midpoint(a, b),
                  bc = CGAL::midpoint(b, c),
                  ca = CGAL::midpoint(c, a);
      size_t ab_idx = addVertex(ab),
             bc_idx = addVertex(bc),
             ca_idx = addVertex(ca);
      std::vector tri_1 = {a_idx,  ab_idx, ca_idx},
                  tri_2 = {ab_idx, b_idx,  bc_idx},
                  tri_3 = {ca_idx, bc_idx, c_idx},
                  tri_4 = {ab_idx, bc_idx, ca_idx};
      new_faces.push_back(std::move(tri_1));
      new_faces.push_back(std::move(tri_2));
      new_faces.push_back(std::move(tri_3));
      new_faces.push_back(std::move(tri_4));
    }
    std::swap(faces_, new_faces);
  }

  Polyhedron toPolyhedron()
  {
    // make a copy as `buildPolyhedron` gets parameters by non-constant reference
    auto points = vertices_;
    auto polygons = faces_;
    return buildPolyhedron(points, polygons);
  }
};

TEST_F(SyntheticSeamCollinearTest, CaseRotation)
{
  Items items = getItems("rotation.x3d");
  ASSERT_FALSE(items.empty());

  std::vector<Polyhedron> polys, refined_polys;
  for (const auto &item : items)
  {
    Polyhedron poly = item.second;

    MyTriMeshRefiner mesh_refiner(poly);
    mesh_refiner.refine();
    mesh_refiner.refine();

    Polyhedron refined_poly = mesh_refiner.toPolyhedron();
    polys.push_back(std::move(poly));
    refined_polys.push_back(std::move(refined_poly));
  }

  std::vector<LineSet> weld_segments, weld_segments_to_unify;
  findSeams(polys, weld_segments);
  findSeams(refined_polys, weld_segments_to_unify);

  ASSERT_FALSE(weld_segments.empty());
  ASSERT_FALSE(weld_segments_to_unify.empty());
  ASSERT_EQ(weld_segments.size(), weld_segments_to_unify.size());

  SearchTree stree1 = makeSearchTree(weld_segments);
  SearchTree stree2 = makeSearchTree(weld_segments_to_unify);

  for (size_t i = 0; i < weld_segments.size(); ++i)
  {
    auto seg = weld_segments[i];
    const Point& p1 = seg.front();
    const Point& p2 = seg.back();
    constexpr double rad = 0.025, eps = 0.0001;
    FuzzySphere fs1(p1, rad, eps);
    FuzzySphere fs2(p2, rad, eps);
    std::vector<Point_and_int> found1, found2, found_intersection;

    stree2.search(std::back_inserter(found1), fs1);
    stree2.search(std::back_inserter(found2), fs2);
    std::set_intersection(found1.begin(), found1.end(), found2.begin(), found2.end(),
                          std::back_inserter(found_intersection),
          [](Point_and_int a, Point_and_int b)
          {
            size_t idx1 = boost::get<1>(a),
                   idx2 = boost::get<1>(b);
            return idx1 < idx2;
          });

    // check 1:1 correspondence between segments in weld_segments and weld_segments_to_unify
    ASSERT_EQ(found_intersection.size(), 1);
    size_t idx_found = boost::get<1>(*found_intersection.begin());
    auto refined_seg = weld_segments_to_unify[idx_found];
    const Point& p1_refined = refined_seg.front();
    const Point& p2_refined = refined_seg.back();

    bool cond = ( (twoPointsEqual(p1, p1_refined) && twoPointsEqual(p2, p2_refined))
                ||(twoPointsEqual(p1, p2_refined) && twoPointsEqual(p2, p1_refined)) );

    ASSERT_TRUE(cond);
  }
}
