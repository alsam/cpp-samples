// -*- C++ -*-

//! \file geom.cpp
//! \brief This file defines implementation for class \c geom.
//!
//! Detailed file description here.
//! \date Saturday, March 13, 2010
//! \author Alexander Samoilov

#include <limits>
#include "geom.h"
#include "get_opt.h"

template <>
void
poly<Polygon_2> :: add (Point_2 const& p)
{
  dlog() << "-D- adding point: " << p << "\n";
  super::push_back(p);
}

template <>
template <>
void
poly<Polygon_2> :: insert<Polygon_2::Vertex_iterator> (Polygon_2::Vertex_iterator it, Point_2 const& p)
{
  dlog() << "-D- inserting additional point: " << p << "\n";
  super::insert(it, p);
}

template <>
void
poly<Polygon_2> :: fix_orientation ()
{
  if (super::orientation() == CGAL::CLOCKWISE) {
    dlog() << "reversing orientation for : ";
    print_polygon(name,*this);
    super::reverse_orientation ();
  }
}

template <>
poly<Polygon_2> :: operator Arrangement_2 ()
{
  Arrangement_2 arr;
  for (Polygon_2::Edge_const_iterator eit = super::edges_begin();
                                      eit != super::edges_end(); ++eit) {
    Segment_2 seg = *eit;
    insert_non_intersecting_curve (arr, seg);
  }

  CGAL_assertion (arr.number_of_faces() == 2);
  for (Arrangement_2::Face_iterator fit = arr.faces_begin();
                                    fit != arr.faces_end(); ++fit) {
    if (fit != arr.unbounded_face()) {
      fit->set_data (face_data(name, type, eps, -1, 0));
    }
  }

  return arr;
}

// An arrangement observer, used to receive notifications of face splits and
// to add overlay of the data to newly created faces.
class Face_data_observer : public CGAL::Arr_observer<Arrangement_2>
{
private:
  int     n_faces;          // The current number of faces.

  face_data fc_data_;

public:

  Face_data_observer (Arrangement_2& arr) :
    CGAL::Arr_observer<Arrangement_2> (arr),
    n_faces (0), fc_data_ ("free_space", face_data::unknown, 0, -1, 0)
  {
    CGAL_precondition (arr.is_empty());
    
    arr.unbounded_face()->set_data (fc_data_);
    n_faces++;
  }

  void set_data (face_data const& fc_data) {fc_data_ = fc_data;}

  virtual void before_split_face (Face_handle old_face,
                                  Halfedge_handle e)
  {
    dlog() << "before_split_face\nold ";

    dump_face (*old_face, n_faces);
    Arrangement_2::Halfedge const& ee = *e;
    Point_2 s = ee.source()->point(), t = ee.target()->point();

    dlog() << "splitting edge: (" << s << "," << t << ")\n";

  }

  virtual void after_split_face (Face_handle old_face,
                                 Face_handle new_face, bool hole)
  {

#if 1

    dlog() << "after_split_face\n";
    dlog() << std::boolalpha << "hole: " << hole << "\n old ";
    dump_face (*old_face, n_faces);

#endif

    // Assign data to the new face.
    face_data fc_data;
    if (old_face->is_unbounded()) {
      fc_data = fc_data_;
    } else {
      face_data_fct face_comp;
      fc_data = face_comp(old_face->data(), fc_data_);
    }

//    if (hole) {
      new_face->set_data (fc_data);
//    } else {
//      new_face->set_data (old_face->data());
//      old_face->set_data (fc_data);
//    }

    dlog() << "\n new ";
    dump_face (*new_face, n_faces);

    n_faces++;
  }


  virtual void before_move_outer_ccb ( Face_handle from_f, Face_handle to_f, Ccb_halfedge_circulator h)
  {
  }

  virtual void after_move_outer_ccb ( Ccb_halfedge_circulator h )
  {
    dlog() << "outer_ccb moved to new face\n";
  }

  virtual void before_move_inner_ccb ( Face_handle from_f, Face_handle to_f, Ccb_halfedge_circulator h)
  {
  }

  virtual void after_move_inner_ccb ( Ccb_halfedge_circulator h )
  {
    dlog() << "inner_ccb moved to new face\n";
  }



};

void
overlay_polygons (Arrangement_2& arr,
                  std::pair<Number_type,Number_type> boundaries,
                  std::vector<planar> const& planar_diels,
                  simple_poly_cont const& polys)
{
  Face_data_observer    obs (arr);

  for (size_t i = 0; i < planar_diels.size(); i++) {
    planar const& pdiel = planar_diels[i];

    Point_2 p1(boundaries.first,  pdiel.from); // left,  bottom
    Point_2 p2(boundaries.second, pdiel.from); // right, bottom
    Point_2 p3(boundaries.second, pdiel.upto); // right, top
    Point_2 p4(boundaries.first,  pdiel.upto); // left,  top
    Segment_2 segs[4] = {Segment_2(p1, p2), Segment_2(p2, p3),
                         Segment_2(p3, p4), Segment_2(p4, p1)};

    obs.set_data (face_data(pdiel.name, face_data::planar, pdiel.eps, i, 0));
    insert (arr, segs, segs+4);
  }

  int id = planar_diels.size() + 10;
  for (simple_poly_cont::const_iterator i = polys.begin(); i != polys.end(); ++i) {
    obs.set_data (face_data(i->name, i->type, i->eps, id++, i->cond_num));
    insert (arr, i->edges_begin(), i->edges_end());
  }

}

void
dump_face (Arrangement_2::Face const& face, int n_faces)
{
  // Go over edges of the face
  if (face.is_unbounded()) {
    dlog() << "face is unbounded\n";
  } else {
    dlog() << "face " << n_faces << " " << face.data().name
           << " eps: "  << CGAL::to_double(face.data().eps)
           << " type: " << face.data().type;

    dlog() << " outer boundary:\n";
    dlog() << "poly(\"" << face.data().name << "\", new pair[]{ "; 
    Arrangement_2::Ccb_halfedge_const_circulator curr = face.outer_ccb();
    dlog() << curr->source()->point();
    do {
      dlog() << " , " << curr->target()->point();
      ++curr;
    } while (curr != face.outer_ccb());

    dlog() << " });\n";
  }
}

void
dump_arrangement_faces (Arrangement_2 const& arr)
{
  for (Arrangement_2::Face_const_iterator fit = arr.faces_begin();
                                          fit != arr.faces_end(); ++fit) {
    dump_face (*fit);
  }
}

template <> point<double>::point (Point_2 const& p)
: std::pair<double,double>(CGAL::to_double(p.x()),CGAL::to_double(p.y()))
{}

void
gen_diel_segments (std::vector<diel_segment<double> >& segs, Arrangement_2 const& arr)
{
  // Print all edges that correspond to an overlapping polyline.
  for (Arrangement_2::Edge_const_iterator eit = arr.edges_begin(); eit != arr.edges_end(); ++eit) {
    Arrangement_2::Halfedge const& e = *eit;
    Arrangement_2::Halfedge const& twin = *e.twin();
    Point_2 s = e.source()->point(), t = e.target()->point();
    face_data const& face_d = e.face()->data();
    face_data const& face_t = twin.face()->data();
    if (face_d.eps != face_t.eps) {
      segs.push_back(diel_segment<double>( s, t, std::make_pair(CGAL::to_double(face_d.eps),
                                                                CGAL::to_double(face_t.eps)),
                                           std::max(face_d.cond_num, face_t.cond_num) ));
    }
  }
}

void
emit_diel_segments_for_n2d (std::ostream &out, std::vector<diel_segment<double> >& segs)
{
}

std::ostream& operator<< (std::ostream& out, const planar& p)
{
  out << "name: "  << p.name
      << " from: " << CGAL::to_double(p.from)
      << " upto: " << CGAL::to_double(p.upto)
      << " eps: "  << CGAL::to_double(p.eps);
  return out;
}


template struct point<double>;
template void poly<Polygon_2> :: add (Point_2 const& p);
template void poly<Polygon_2> :: fix_orientation ();
template poly<Polygon_2> :: operator Arrangement_2 ();
template void poly<Polygon_2> :: insert<Polygon_2::Vertex_iterator> (Polygon_2::Vertex_iterator, Point_2 const& p);

