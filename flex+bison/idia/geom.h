#ifndef __INCL_geom_H__
#define __INCL_geom_H__
// prevent multiple includes

// -*- C++ -*-

//! \file geom_basic.hpp
//! \brief This file defines implementation for class \c geom_basic.
//!
//! Detailed file description here.
//! \date Wednesday, March 11, 2010
//! \author Alexander Samoilov

// -------------- Forward declarations and includes -------------------
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <vector>
#include <list>
#include <set>

// CGAL headers
#include "CGAL/Cartesian.h"
#include "CGAL/Boolean_set_operations_2.h"
#include "CGAL/Polygon_set_2.h"
#include "CGAL/Arrangement_2.h"
#include "CGAL/Arr_segment_traits_2.h"
#include "CGAL/Arrangement_2.h"
#include "CGAL/Arr_extended_dcel.h"
#include "CGAL/Arr_overlay_2.h"
#include "CGAL/Arr_default_overlay_traits.h"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include "CGAL/Gmpq.h"
#include "CGAL/enum.h"

#include "get_opt.h"

#if 1 // defined EXACT

typedef CGAL::Gmpq Number_type;
typedef CGAL::Cartesian<Number_type>                            Kernel;

#else // below is slightly faster

typedef CGAL::Exact_predicates_exact_constructions_kernel  Kernel;
//typedef CGAL::Exact_predicates_inexact_constructions_kernel  Kernel;
typedef Kernel::FT                                         Number_type;

#endif

typedef CGAL::Arr_segment_traits_2<Kernel>                      Traits_2;
//typedef CGAL::Arr_non_caching_segment_traits_2<Kernel>                      Traits_2;
typedef Traits_2::Point_2                                       Point_2;
typedef Traits_2::X_monotone_curve_2                            Segment_2;

// face_data class and functor

struct face_data {

  enum face_type {unknown, planar, conformal, conductor,};

  std::string name;

  Number_type eps; // dielectrical permittivity

  face_type type;

  int id, cond_num;

  // a constructor
  face_data () : type(unknown), eps(0) {}

  face_data (std::string const& aName, face_type aType, Number_type const& aEps, int aId, int aCondNum)
    : name(aName), type(aType), eps(aEps), id(aId), cond_num(aCondNum) {}

};

struct face_data_fct {
  face_data operator () (face_data const& left, face_data const& right) const {

    if (left.type == face_data::conductor) // right is conformal, planar or unknown
      return left;

    if (right.type == face_data::conductor)  // left is conformal, planar or unknown
      return right;

    if (left.type == face_data::conformal && right.type == face_data::conformal) {
      return (left.id < right.id) ? left : right;
    }

    if (left.type == face_data::conformal) // right is planar or unknown
      return left;

    if (right.type == face_data::conformal) // left is planar or unknown
      return right;

    if (left.type == face_data::planar) // right is unknown
      return left;

    if (right.type == face_data::planar) // left is unknown
      return right;

    return left;
    //throw "face_data_fct::operator() : impossible happened";
  }
};

typedef CGAL::Arr_face_extended_dcel<Traits_2, face_data>       Dcel;
typedef CGAL::Arrangement_2<Traits_2, Dcel>                     Arrangement_2;

typedef CGAL::Arr_face_overlay_traits<Arrangement_2, Arrangement_2, Arrangement_2,
                                      face_data_fct>            Overlay_traits;


typedef CGAL::Polygon_2<Kernel /*, std::list<Point_2>*/ >                         Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel /*, std::list<Point_2>*/ >              Polygon_with_holes_2;
typedef CGAL::Polygon_set_2<Kernel /*, std::list<Point_2>*/ >                     Polygon_set_2;
typedef std::list<Polygon_with_holes_2>                 Pwh_list_2;

//-----------------------------------------------------------------------------
// Pretty-print a CGAL polygon.
//
template<class Kernel, class Container>
void print_polygon (std::string const& header, CGAL::Polygon_2<Kernel, Container> const& P)
{
  typename CGAL::Polygon_2<Kernel, Container>::Vertex_const_iterator  vit;

  std::string ori;
  switch (P.orientation()) {
    case CGAL::CLOCKWISE:        ori = "CLOCKWISE"; break;
    case CGAL::COUNTERCLOCKWISE: ori = "COUNTERCLOCKWISE"; break;
    default: ori = "???";
  }

  dlog() << header << " " << "orientation: " << ori << " " << P.size() << " vertices: {";
  for (vit = P.vertices_begin(); vit != P.vertices_end(); ++vit)
    dlog() << '(' << CGAL::to_double(vit->x()) << ", " << CGAL::to_double(vit->y()) << "), ";
  dlog() << " }" << std::endl;

}

//-----------------------------------------------------------------------------
// Pretty-print a polygon with holes.
//
template<class Kernel, class Container>
void print_polygon_with_holes
    (const CGAL::Polygon_with_holes_2<Kernel, Container>& pwh)
{
  if (! pwh.is_unbounded()) {
    dlog() << "{ Outer boundary = ";
    print_polygon ("", pwh.outer_boundary());
  }
  else
    dlog() << "{ Unbounded polygon." << std::endl;

  typename CGAL::Polygon_with_holes_2<Kernel,Container>::Hole_const_iterator  hit;
  size_t k = 1;

  dlog() << "  " << pwh.number_of_holes() << " holes:" << std::endl;
  for (hit = pwh.holes_begin(); hit != pwh.holes_end(); ++hit, ++k) {
    dlog() << "    Hole #" << k << " = ";
    print_polygon ("", *hit);
  }
  dlog() << " }" << std::endl;

}

struct planar {

  //typedef double value_type;
  typedef Number_type value_type;

  std::string name;
  Number_type eps;
  value_type from, upto;

  planar(std::string name, Number_type eps=0, value_type from=0, value_type upto=0)
   : name(name), eps(eps), from(from), upto(upto) {}

  //! accessors : for interface convenience
  value_type min () const {return from;}

  value_type max () const {return upto;}

  template <typename Other>
  bool operator < (Other const& other) const {
    return min() < other.min();
  }

};

template <typename poly_base>
struct poly : public poly_base
{
  typedef poly_base super;

  typedef Number_type value_type;

  std::string name;

  Number_type eps;

  //! a conductor number, starting from one, zero for dielectric
  int cond_num;

  face_data::face_type type;


  //! a constructor
  poly () : eps(0), cond_num(0) {}

  //! another constructor
  poly (poly_base const& base) : poly_base(base) {}

  void add (Point_2 const&);

  /**
   * precondition : inserted point splits some edge, there is no update of min and max y
   */
  template <typename Iterator>
  void insert (Iterator iterator, Point_2 const&);

  void reset ();

  //! for correct Boolean set operations on polygons outer boundary
  //! should have counterclockwise orientation
  //! holes - clockwise
  //! apply this method only for input polygons
  void fix_orientation ();

  operator Arrangement_2 ();

};

template <typename Base>
void
poly<Base> :: reset ()
{
  super::clear ();
}

typedef poly<Polygon_2> simple_poly;
typedef poly<Polygon_with_holes_2> poly_with_holes;
typedef std::vector<simple_poly> simple_poly_cont;

std::ostream& operator<< (std::ostream& out, const planar& p);


void
overlay_polygons (Arrangement_2& arr,
                  std::pair<Number_type,Number_type> boundaries,
                  std::vector<planar> const& planar_diels,
                  simple_poly_cont const& polys);

void
dump_face (Arrangement_2::Face const&, int = -1);

void
dump_arrangement_faces (Arrangement_2 const& arr);

// --------------------------------------------------------------------
//! An auxiliary parameterized class for storing geometric data:
//! point as a pair of numbers
template <typename T>
struct point : public std::pair<T,T>
{
  //! a default constructor
  point () : std::pair<T,T>(0,0) {}

  //! a constructor
  point (T x, T y) : std::pair<T,T>(x,y) {}

  //! a constructor
  point (Point_2 const& p);

  //! abscissa x
  T const& x() const {return this->first;}

  //! ordinate y
  T const& y() const {return this->second;} 
};

// --------------------------------------------------------------------
//! An auxiliary parameterized class for storing geometric data:
//! segment as a pair of points
template <typename T>
struct segment : public std::pair<point<T>, point<T> >
{
  //! a constructor
  segment (point<T> const& source, point<T> const& target)
  : std::pair<point<T>,point<T> >(source,target) {
    if (source == target) {
      std::ostringstream msg;
      msg << "empty segments with source == target: " << source
          << " are not permitted";
      throw std::runtime_error(msg.str());
    }
  }

  //! data accessor: a source end of the segment
  point<T> const& source () const {return this->first;}

  //! data accessor: a second end of the segment
  point<T> const& target () const {return this->second;}

  virtual segment& ordered () {
    if (target() < source()) {
      std::swap(this->first, this->second);
    }
    return *this;
  }
};

template <typename T>
struct diel_segment : public segment<T>
{
  std::pair<T,T> eps;

  int cond_num;

  //! a constructor
  diel_segment (point<T> const& source, point<T> const& target,
                std::pair<T,T> const& e, int cnum)
    : segment<T>(source, target), eps(e), cond_num(cnum) {}

  virtual diel_segment& ordered () {
    if (this->target() < this->source()) {
      std::swap(this->first, this->second);
      std::swap(eps.first, eps.second);
    }
    return *this;
  }

  virtual ~diel_segment () {}

};


void
gen_diel_segments (std::vector<diel_segment<double> >& segs, Arrangement_2 const& arr);

void
emit_diel_segments_for_n2d (std::ostream &out, std::vector<diel_segment<double> >& segs);

template <typename T>
inline std::ostream& operator<< (std::ostream &out, point<T> const& p)
{
  out << "(" << p.x() << "," << p.y() << ")";
  return out;
}

template <typename T>
inline std::ostream& operator<< (std::ostream &out, diel_segment<T> const& ds)
{
  out << ds.source() << "--" << ds.target() << " eps : (" << ds.eps.first << "," << ds.eps.second << ")"
      << " cond_num: " << ds.cond_num;
  return out;
}

inline std::ostream& operator<< (std::ostream &out, Point_2 const& p)
{
  out << "(" << CGAL::to_double(p.x()) << ", " << CGAL::to_double(p.y()) << ")";
  return out;
}

inline std::ostream& operator<< (std::ostream &out, Segment_2 const& s)
{
  out << "(" << s.source() << ", " << s.target() << ")";
  return out;
}

// prevent multiple includes
#endif
