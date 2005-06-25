// Copyright (c) 2005 Foundation for Research and Technology-Hellas (Greece).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Menelaos Karavelas <mkaravel@tem.uoc.gr>

#ifndef CGAL_VORONOI_DIAGRAM_2_HALFEDGE_2_H
#define CGAL_VORONOI_DIAGRAM_2_HALFEDGE_2_H 1

#include <CGAL/Voronoi_diagram_adaptor_2/basic.h>
#include <CGAL/Voronoi_diagram_adaptor_2/Finder_classes.h>
#include <CGAL/Triangulation_utils_2.h>

CGAL_BEGIN_NAMESPACE

CGAL_VORONOI_DIAGRAM_2_BEGIN_NAMESPACE

template<class VDA>
class Halfedge
{
 private:
  typedef Halfedge<VDA>                         Self;
  typedef Triangulation_cw_ccw_2                CW_CCW_2;

  typedef typename VDA::Dual_face_handle        Dual_face_handle;
  typedef typename VDA::Dual_vertex_handle      Dual_vertex_handle;
  typedef typename VDA::Dual_vertex_circulator  Dual_vertex_circulator;
  typedef typename VDA::Edge_degeneracy_tester  Edge_degeneracy_tester;
  typedef typename VDA::Face_degeneracy_tester  Face_degeneracy_tester;

 private:
  Self find_next_1D() const {
    Dual_vertex_circulator vc = vda_->dual().incident_vertices(v1_);

    while ( v2_ == vc ) { ++vc; }

    if ( vda_->dual().is_infinite(vc) ) { return *this; }

    return Self(vda_, v1_, vc);
  }

  void find_next(const Dual_face_handle& f, int i,
		 Dual_face_handle& fnext, int& inext) const
  {
    Find_next_halfedge<VDA>()(vda_, f, i, fnext, inext);
  }

  void find_opposite(const Dual_face_handle& f, int i,
		     Dual_face_handle& fopp, int& iopp) const 
  {
    Find_opposite_halfedge<VDA>()(vda_, f, i, fopp, iopp);
  }

public:
  typedef typename VDA::Vertex                   Vertex;
  typedef typename VDA::Face                     Face;
  typedef typename VDA::Vertex_handle            Vertex_handle;
  typedef typename VDA::Face_handle              Face_handle;
  typedef typename VDA::Halfedge_handle          Halfedge_handle;
  typedef typename VDA::Ccb_halfedge_circulator  Ccb_halfedge_circulator;

  typedef typename VDA::Voronoi_traits::Curve    Curve;

  typedef typename VDA::Dual_graph               Dual_graph;
  typedef typename VDA::Dual_graph::Edge         Dual_edge;

  Halfedge(const VDA* vda = NULL)
    : vda_(vda), f_(Dual_face_handle()), i_(-1),
      v1_(Dual_vertex_handle()), v2_(Dual_vertex_handle()) {}
  Halfedge(const VDA* vda, Dual_face_handle f, int i)
    : vda_(vda), f_(f), i_(i), v1_(Dual_vertex_handle()),
      v2_(Dual_vertex_handle())
  {
#if !defined(CGAL_NO_PRECONDITIONS) && !defined(NDEBUG)
    CGAL_precondition( vda_->dual().dimension() == 2 );
    CGAL_precondition( !vda_->edge_tester()(vda_->dual(), f_, i_) );
#if 0
    Dual_vertex_handle v = f_->vertex( CW_CCW_2::ccw(i_) );
    // the following test prohibit creating a halfedge which is
    // temporarily invalid...; this creates a problem for the
    // segment Voronoi diagram
    CGAL_precondition( !vda_->face_tester()(vda_->dual(), v) );
#endif
#endif
  }

  Halfedge(const VDA* vda, Dual_vertex_handle v1, Dual_vertex_handle v2)
    : vda_(vda), f_(Dual_face_handle()), i_(-2), v1_(v1), v2_(v2)
  {
    CGAL_precondition( vda_->dual().dimension() == 1 );
    CGAL_precondition( !vda_->dual().is_infinite(v1_) );
  }

  Halfedge_handle opposite() const {
    if ( vda_->dual().dimension() == 1 ) {
      return Halfedge_handle( Self(vda_, v2_, v1_) );
    }

    int cw_i = CW_CCW_2::cw(i_);
    if (  vda_->face_tester()(vda_->dual(), f_->vertex(cw_i))  ) {
      Dual_face_handle fopp;
      int iopp;
      find_opposite(f_, i_, fopp, iopp); //equivalent to: twin().next().twin();
      return Halfedge_handle( Self(vda_, fopp, iopp) );
    } else {
      int i_mirror = vda_->dual().tds().mirror_index(f_, i_);
      return
	Halfedge_handle( Self(vda_, f_->neighbor(i_), i_mirror) );
    }
  }

  inline Halfedge_handle twin() const {
    return opposite();
  }

#if 0
  Halfedge_handle next_halfedge() const {
    return next();
  }
#endif

  Halfedge_handle next() const {
    if ( vda_->dual().dimension() == 1 ) {
      return Halfedge_handle( find_next_1D() );
    }

    // if I want to return all edges and not just the finite ones,
    // replace the do-while loop by the following statements:
    //          find_next(f_, i_, f, i);
    Dual_face_handle f = f_, fnext;
    int i = i_, inext;
    do {
      find_next(f, i, fnext, inext);
      f = fnext;
      i = inext;
    } while ( vda_->dual().is_infinite(f, i) );
    return Halfedge_handle( Self(vda_, f, i) );
  }

  Halfedge_handle previous() const
  {
    if ( vda_->dual().dimension() == 1 ) {
      return Halfedge_handle( find_next_1D() );
    }

    Dual_face_handle f, fprev = f_;
    int iprev = i_, i;
    
    // if I want to return also infinite edges replace the test in
    // the while loop by the following test (i.e., should omit the
    // testing for infinity):
    //           vda_->edge_tester()(vda_->dual(), f, i)
    do {
      f = fprev->neighbor(iprev);
      int i_mirror = vda_->dual().tds().mirror_index(fprev, iprev);
      i = CW_CCW_2::ccw( i_mirror );
      fprev = f;
      iprev = i;
    } while ( vda_->edge_tester()(vda_->dual(), f, i) ||
	      vda_->dual().is_infinite(f, i) );

    return Halfedge_handle( Self(vda_, f, i) );
  }

  Ccb_halfedge_circulator ccb() const {
    return Ccb_halfedge_circulator( *this );
  }

  bool operator==(const Self& other) const {
    if ( vda_ == NULL ) { return other.vda_ == NULL; }
    if ( other.vda_ == NULL ) { return vda_ == NULL; }

    if ( vda_->dual().dimension() == 1 ) {
      return ( vda_ == other.vda_ && v1_ == other.v1_ && v2_ == other.v2_ );
    } else {
      return ( vda_ == other.vda_ && f_ == other.f_ && i_ == other.i_ );
    }
  }
  
  bool operator!=(const Self& other) const {
    return !((*this) == other);
  }

  bool has_source() const {
    return opposite()->has_target();
  }

  bool has_target() const {
    if ( vda_->dual().dimension() == 1 ) { return false; }
    return !vda_->dual().is_infinite(f_);
  }

  bool is_unbounded() const {
    return !has_source() || !has_target();
  }

  Vertex_handle source() const {
    CGAL_precondition( has_source() );
    return opposite()->vertex();
  }

  Vertex_handle target() const {
    CGAL_precondition( has_target() );
    return vertex();
  }

 private:
  Vertex_handle vertex() const {
    Dual_face_handle fvalid = Find_valid_vertex<VDA>()(vda_, f_);
    CGAL_assertion( !vda_->dual().is_infinite(fvalid) );
    return Vertex_handle( Vertex(vda_, fvalid) );
  }

 public:
  Face_handle face() const {
    if ( vda_->dual().dimension() == 1 ) {
      return Face_handle( Face(vda_, v1_) );
    }
    Face f(vda_, f_->vertex( CW_CCW_2::ccw(i_) ));
    return Face_handle(f);
    //      Face_handle(   Face(vda_, f_->vertex( CW_CCW_2::ccw(i_) ))   );
  }

  Curve curve() const {
    if ( vda_->dual().dimension() == 1 ) {
      return VDA::Voronoi_traits::make_edge(v1_, v2_);
    }

    Dual_vertex_handle v1 = f_->vertex( CW_CCW_2::cw(i_) ); // north
    Dual_vertex_handle v2 = f_->vertex( CW_CCW_2::ccw(i_) ); // south
    Dual_vertex_handle v3 = f_->vertex( i_ ); // west
    Dual_vertex_handle v4 = vda_->dual().tds().mirror_vertex(f_,i_); // east

    CGAL_precondition( !vda_->dual().is_infinite(v1) );
    CGAL_precondition( !vda_->dual().is_infinite(v2) );

    bool is_inf3 = vda_->dual().is_infinite(v3);
    bool is_inf4 = vda_->dual().is_infinite(v4);

    if ( is_inf3 && is_inf4 ) {
      return VDA::Voronoi_traits::make_edge(v1, v2);
    } else if ( is_inf3 ) {
      return VDA::Voronoi_traits::make_edge(v1, v2, v4, false);
    } else if ( is_inf4 ) {
      return VDA::Voronoi_traits::make_edge(v1, v2, v3, true);
    } else {
      return VDA::Voronoi_traits::make_edge(v1, v2, v3, v4);
    }
  }

  Dual_edge dual_edge() const {
    if ( vda_->dual().dimension() == 1 ) {
      typename VDA::Dual_graph::Edge_circulator ec;
      if ( vda_->dual().is_infinite(v1_) ) {
	CGAL_assertion( !vda_->dual().is_infinite(v2_) );
	ec = vda_->dual().incident_edges(v2_);
      } else {
	ec = vda_->dual().incident_edges(v1_);
      }
      do {
	Dual_edge e = *ec;
	if ( (e.first->vertex(0) == v1_ && e.first->vertex(1) == v2_) ||
	     (e.first->vertex(0) == v2_ && e.first->vertex(1) == v1_) ) {
	  return e;
	}
	++ec;
      } while ( true );
    } else {
      return Dual_edge(f_, i_);
    }
  }

  bool is_valid() const {
    if ( vda_ == NULL ) { return true; }

    bool valid = true;

    if ( vda_->dual().dimension() == 1 ) {
      valid = valid && v1_ != Dual_vertex_handle();
      valid = valid && v2_ != Dual_vertex_handle();
    } else {
      valid = valid && !vda_->edge_tester()(vda_->dual(), f_, i_);

      Dual_vertex_handle v = f_->vertex( CW_CCW_2::ccw(i_) );

      valid = valid && !vda_->face_tester()(vda_->dual(), v);
    }

    Halfedge_handle h_this(*this);

    valid = valid && opposite()->opposite() == h_this;

    if ( has_source() ) {
      valid = valid && source()->is_valid();
      valid = valid && source()->is_incident_edge( h_this );
    }

    if ( has_target() ) {
      valid = valid && target()->is_valid();
      valid = valid && target()->is_incident_edge( h_this );
    }

    valid = valid && next()->previous() == h_this;
    valid = valid && previous()->next() == h_this;
    return valid;
  }

 private:
  const VDA* vda_;
  Dual_face_handle f_;
  int i_;
  Dual_vertex_handle v1_, v2_;
};

CGAL_VORONOI_DIAGRAM_2_END_NAMESPACE

CGAL_END_NAMESPACE

#endif // CGAL_VORONOI_DIAGRAM_2_HALFEDGE_2_H
