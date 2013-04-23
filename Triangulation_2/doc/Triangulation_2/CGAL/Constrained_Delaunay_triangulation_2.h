
namespace CGAL {

/*!
\ingroup PkgTriangulation2TriangulationClasses

A constrained Delaunay triangulation is a triangulation with
constrained edges which tries to be as much Delaunay as possible.
Constrained edges are not necessarily Delaunay edges, therefore a
constrained Delaunay triangulation is not a Delaunay triangulation. A
constrained Delaunay is a triangulation whose faces do not necessarily
fulfill the empty circle property but fulfill a weaker property called
the <I>constrained empty circle</I>.  To state this property, it is
convenient to think of constrained edges as blocking the view. Then, a
triangulation is constrained Delaunay if the circumscribing circle of
any of its triangular faces includes in its interior no vertex that is
visible from the interior of the triangle.

As in the case of constrained triangulations, three different versions 
of Delaunay constrained triangulations are offered 
depending on whether the user wishes to handle 
intersecting input constraints or not. 

\tparam Traits is the geometric traits of a constrained Delaunay triangulation.
It must be a model of `DelaunayTriangulationTraits_2`, providing the `side_of_oriented_circle` test 
of a Delaunay triangulation. 
When intersection of input constraints are supported, 
the geometric traits class 
is required to provide additional function object types 
to compute the intersection of two segments. 
and has then to be also a model of the concept 
`ConstrainedTriangulationTraits_2`. 

\tparam Tds must be a model of `TriangulationDataStructure_2`. 

\tparam Itag allows to select if intersecting constraints are supported and how they are handled.
- `No_intersection_tag` if intersections of 
input constraints are disallowed, 
- `Exact_predicates_tag` allows intersections between input
constraints and is to be used when the traits class provides exact
predicates but approximate constructions of the intersection points,
- `Exact_intersections_tag` allows intersections between input
constraints and is to be used in conjunction with an exact arithmetic
type.


A constrained Delaunay triangulation is not a Delaunay 
triangulation but it is a constrained triangulation. 
Therefore the class `Constrained_Delaunay_triangulation_2` derives from 
the class `Constrained_triangulation_2<Traits,Tds>`. 
Also, information about the status (constrained or not) 
of the edges of the triangulation is stored 
in the faces. 
Thus the nested `Face` 
type of a constrained triangulation offers 
additional functionalities to deal with this information. 
These additional functionalities 
induce additional requirements on the base face class 
plugged into the triangulation data structure of 
a constrained Delaunay triangulation. 
The base face of a constrained Delaunay triangulation 
has to be a model of the concept 
`ConstrainedTriangulationFaceBase_2`. 

\cgal provides a default for the template parameters. 
If `Gt` is the geometric traits 
parameter, 
the default for 
`ConstrainedTriangulationFaceBase_2` is the class 
`Constrained_triangulation_face_base_2<Gt>` 
and the default for the 
triangulation data structure parameter is the class 
`Triangulation_data_structure_2< CGAL::Triangulation_vertex_base_2<Gt>, Constrained_triangulation_face_base_2<Gt> >`. 
The default intersection tag is `No_intersection_tag`. 

\cgalHeading{Types}


All types used in this class are inherited from the base class 
`Constrained_triangulation_2<Traits,Tds,Itag>`. 

\sa `CGAL::Constrained_triangulation_2<Traits,Tds,Itag>` 
\sa `TriangulationDataStructure_2` 
\sa `DelaunayTriangulationTraits_2` 
\sa `ConstrainedTriangulationTraits_2` 
\sa `ConstrainedDelaunayTriangulationTraits_2` 
\sa `ConstrainedTriangulationFaceBase_2` 

*/
template< typename Traits, typename Tds, typename Itag >
class Constrained_Delaunay_triangulation_2 : public Constrained_triangulation_2<Traits,Tds,Itag> {
public:

/// \name Creation 
/// @{

/*! 
Introduces an empty constrained Delaunay triangulation `cdt`. 
*/ 
Constrained_Delaunay_triangulation_2(const Traits &t = Traits()); 

/*! 
Copy constructor: All faces and vertices 
are duplicated and the constrained status of edges 
is copied. 
*/ 
Constrained_Delaunay_triangulation_2(const 
Constrained_Delaunay_triangulation_2& cdt1); 

/*! 
A templated constructor which introduces and builds 
a constrained triangulation with constrained edges in the range 
`[first,last)`. 
\tparam InputIterator must be an input iterator with the value type `Constraint`. 
*/ 
template<class InputIterator> Constrained_Delaunay_triangulation_2( 
InputIterator first, 
InputIterator last, 
const Traits& t=Traits()); 

/// @} 

/// \name Insertion and Removal 
/// The following member functions overwrite the corresponding members
/// of the base class to include a step restoring the Delaunay
/// constrained property after modification of the triangulation.
/// @{

/*! 
Inserts point `p` in the triangulation, with face
`f` as a hint for the location of `p`. 
*/ 
Vertex_handle insert(Point p, Face_handle f = Face_handle()); 

/*! 
Inserts point `p` in the triangulation at the location given by `(lt,loc,i)`. 
\sa `Triangulation_2::locate()`
*/ 
Vertex_handle 
insert(const Point& p, 
Locate_type lt, 
Face_handle loc, int li ); 

/*! 
Equivalent to `insert(p)`. 
*/ 
Vertex_handle push_back(const Point& p); 

/*! 
Inserts the points in the range `[first,last)`. 
Returns the number of inserted points. 
\tparam InputIterator must be an input iterator with the value type `Point`. 
*/ 
template < class InputIterator > 
std::ptrdiff_t 
insert(InputIterator first, InputIterator last); 

/*!
inserts the points in the iterator range `[first,last)`. Returns the number of inserted points.
Note that this function is not guaranteed to insert the points
following the order of `PointWithInfoInputIterator`, as `spatial_sort()`
is used to improve efficiency.
Given a pair `(p,i)`, the vertex `v` storing `p` also stores `i`, that is
`v.point() == p` and `v.info() == i`. If several pairs have the same point,
only one vertex is created, and one of the objects of type `Vertex::Info` will be stored in the vertex.
\pre `Vertex` must be model of the concept `TriangulationVertexBaseWithInfo_2`.

\tparam PointWithInfoInputIterator must be an input iterator with the value type `std::pair<Point,Vertex::Info>`.
*/ 
template < class PointWithInfoInputIterator >
std::ptrdiff_t
insert(PointWithInfoInputIterator first, PointWithInfoInputIterator last);

/*! 
Inserts segment `ab` as a constrained edge in the triangulation. 
*/ 
void insert_constraint(Point a, Point b); 

/*! 
Inserts the line segment between the points `c.first` and `c.second` as  a constrained edge in the triangulation.
*/ 
void push_back(const Constraint& c); 

/*! 
Inserts the line segment whose endpoints are the vertices `va` and 
`vb` as a constrained edge e in the triangulation. 
*/ 
void insert_constraint(Vertex_handle va, Vertex_handle vb); 

/*! 
Removes vertex v. 
\pre Vertex `v` is not incident to a constrained edge. 
*/ 
void remove(Vertex_handle & v); 

/*! 
Make the edges incident to vertex `v` unconstrained edges. 
*/ 
void remove_incident_constraints(Vertex_handle v); 

/*! 
Make the edge `(f,i)` unconstrained. 
*/ 
void remove_constraint(const Face_handle & f, int i); 

/// @} 

/// \name Queries 
/// The following template member functions query the set of faces in
/// conflict with a point `p`. The notion of conflict refers here to a
/// constrained Delaunay setting which means the
/// following. Constrained edges are considered as visibility
/// obstacles and a point `p` is considered to be in conflict with a
/// face `f` iff it is visible from the interior of `f` and included
/// in the circumcircle of `f`.
/// @{

/*! 
outputs the faces and boundary edges of the conflict zone of point `p` into
output iterators.

This function outputs in the container pointed to by `fit` 
the faces which are in conflict with point `p`. 
It outputs in the container pointed to by `eit` 
the boundary of the zone in conflict with `p`. 
The boundary edges 
of the conflict zone are output in counterclockwise order 
and each edge is described through its incident face 
which is not in conflict with `p`. 
The function returns in a `std::pair` the resulting output iterators. 

\tparam OutItFaces is an output iterator with `Face_handle` as value type. 
\tparam OutItBoundaryEdges is an output iterator with `Edge` as value type. 

\pre `dimension()==2`. 
*/ 
template <class OutputItFaces, class OutputItBoundaryEdges> 
std::pair<OutputItFaces,OutputItBoundaryEdges> 
get_conflicts_and_boundary(const Point &p, 
OutputItFaces fit, 
OutputItBoundaryEdges eit, 
Face_handle start) const; 

/*! 
outputs the faces of the conflict zone of point `p` into an output iterator.

Same as `get_conflicts_and_boundary` except that only the faces in conflict with `p` 
are output. The function returns the resulting output iterator. 
\pre `dimension()==2`. 
*/ 
template <class OutputItFaces> 
OutputItFaces 
get_conflicts (const Point &p, 
OutputItFaces fit, 
Face_handle start) const; 

/*! 
outputs the boundary edges of the conflict zone of point `p` into an output iterator.

This functions outputs in the container pointed to by `eit`, 
the boundary of the zone in conflict with `p`. The boundary edges 
of the conflict zone are output in counter-clockwise order 
and each edge is described through the incident face 
which is not in conflict with `p`. 
The function returns the resulting output iterator. 
\tparam OutputItBoundaryEdges is an output iterator with 
`Edge` as value type. 
*/ 
template <class OutputItBoundaryEdges> 
OutputItBoundaryEdges 
get_boundary_of_conflicts(const Point &p, 
OutputItBoundaryEdges eit, 
Face_handle start) const; 

/// @} 

/// \name Checking 
/// @{

/*! 
checks if the triangulation is valid and if each constrained edge is 
consistently marked constrained in its two incident faces. 
*/ 
bool is_valid() const; 

/// @} 

/// \name Flips 
/// @{

/*! 
determines if edge `(f,i)` can be flipped.
\cgalAdvanced  Returns true if 
edge `(f,i)` is not constrained and the circle circumscribing `f` 
contains the vertex of `f->neighbor(i)` 
opposite to edge `(f,i)`. 
*/ 
bool is_flipable(Face_handle f, int i) const; 

/*! 
\cgalAdvanced Flip `f` and `f->neighbor(i)`. 
*/ 
void flip(Face_handle& f, int i); 

/*! 
makes the triangulation constrained Delaunay by flipping
edges.
\cgalAdvanced   The list `edges` contains an initial list of edges to be
flipped. The returned triangulation is constrained Delaunay if the
initial list contains at least all the edges of the input
triangulation that failed to be constrained Delaunay. (An edge is said
to be constrained Delaunay if it is either constrained or locally
Delaunay.)
*/ 
void propagating_flip(List_edges & edges); 

/// @}

}; /* end Constrained_Delaunay_triangulation_2 */
} /* end namespace CGAL */