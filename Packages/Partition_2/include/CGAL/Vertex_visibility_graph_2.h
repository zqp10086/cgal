// ============================================================================
//
// Copyright (c) 2000 The CGAL Consortium
//
// This software and related documentation is part of an INTERNAL release
// of the Computational Geometry Algorithms Library (CGAL). It is not
// intended for general use.
//
// ----------------------------------------------------------------------------
//
// release       : $CGAL_Revision $
// release_date  : $CGAL_Date $
//
// file          : include/CGAL/Vertex_visibility_graph_2.h
// package       : $CGAL_Package: Partition_2 $
// maintainer    : Susan Hert <hert@mpi-sb.mpg.de>
// chapter       : Planar Polygon Partitioning
//
// revision      : $Revision$
// revision_date : $Date$
//
// author(s)     : Susan Hert <hert@mpi-sb.mpg.de>
//
// coordinator   : MPI (Susan Hert <hert@mpi-sb.mpg.de>)
//
// implementation: Polygon vertex visibility graph
// ============================================================================

#ifndef  CGAL_VERTEX_VISIBILITY_GRAPH_2_H
#define  CGAL_VERTEX_VISIBILITY_GRAPH_2_H

#include <CGAL/Segment_2.h>
#include <CGAL/Rotation_tree_2.h>
#include <CGAL/Indirect_less_xy_compare_2.h>
#include <CGAL/Iterator_list.h>
#include <CGAL/Turn_reverser.h>
#include <CGAL/Point_pair_less_xy_2.h>
#include <CGAL/Segment_2_Ray_2_intersection.h>
#include <CGAL/Segment_less_yx_2.h>
#include <cmath>
#include <list>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <iostream>

namespace CGAL {

template <class Traits>
class Vertex_visibility_graph_2 
{
private:
   typedef typename Traits::R                 R;
   typedef typename Traits::Point_2           Point_2;
   typedef typename Traits::Segment_2         Segment_2;
   typedef typename Traits::Ray_2             Ray_2;
   typedef typename Traits::Leftturn_2        Leftturn_2;
   typedef typename Traits::Less_xy_2         Less_xy_2;
   typedef typename Traits::Orientation_2     Orientation_2;
   typedef typename Traits::Collinear_are_ordered_along_line_2 
                                             Collinear_are_ordered_along_line_2;
   typedef typename Traits::Are_strictly_ordered_along_line_2 
                                             Are_strictly_ordered_along_line_2;
   typedef typename Traits::Construct_segment_2 Construct_segment_2; 
   typedef typename Traits::Construct_ray_2     Construct_ray_2; 
   typedef Segment_less_yx_2<Traits>          Segment_less_yx_2;

   typedef Rotation_tree_2<Traits>            Tree;
   typedef typename Tree::iterator            Tree_iterator;

   typedef std::list< Point_2 >               Polygon_2;
   typedef typename Polygon_2::const_iterator Polygon_const_iterator;
   typedef typename Polygon_2::iterator       Polygon_iterator;

   // the edge set is simply a set of point pairs.
   typedef std::pair<Point_2, Point_2>                Point_pair;
   typedef Point_pair_less_xy_2<Traits>               Point_pair_compare;
   typedef std::set< Point_pair, Point_pair_compare > Edge_set;

   // this map associates with each point (vertex), the iterator in the
   // original list that it originated from and its current visibility
   // point iterator. 
   typedef std::pair<Polygon_const_iterator, Polygon_const_iterator>   
                                               Iterator_pair;
   typedef std::map<Point_2, Iterator_pair, Less_xy_2>     Vertex_map;
   typedef typename Vertex_map::iterator                   Vertex_map_iterator;

public:
   typedef typename Edge_set::iterator                iterator;
   typedef typename Edge_set::const_iterator          const_iterator;

   Vertex_visibility_graph_2()  {}

   //
   // first and beyond should be iterators over vertices of a polygon
   //
   template <class ForwardIterator>
   Vertex_visibility_graph_2(ForwardIterator first, ForwardIterator beyond):
     leftturn_2(Traits().leftturn_2_object()), 
     orientation_2(Traits().orientation_2_object()), 
     collinear_ordered_2(Traits().collinear_are_ordered_along_line_2_object()), 
     are_strictly_ordered_along_line_2(Traits().are_strictly_ordered_along_line_2_object()),
     less_xy_2(Traits().less_xy_2_object()),
     construct_segment_2(Traits().construct_segment_2_object()),
     construct_ray_2(Traits().construct_ray_2_object())
   {
       build(first, beyond);
   }

   // Pre:  ccw order of points; no repeated points
   template <class ForwardIterator>
   void build(ForwardIterator first, ForwardIterator beyond);

   void clear()
   {
      edges.clear();
   }

   iterator begin()
   {
      return edges.begin();
   }

   const_iterator begin() const
   {
      return edges.begin();
   }

   const_iterator end() const
   {
      return edges.end();
   }

   iterator end()
   {
      return edges.end();
   }

   void insert_edge(const Point_pair& edge)
   {
#ifdef CGAL_VISIBILITY_GRAPH_DEBUG
       std::pair<iterator, bool> result = 
#endif
       if (less_xy_2(edge.first,edge.second))
          edges.insert(edge);
       else
          edges.insert(Point_pair(edge.second, edge.first));
#ifdef CGAL_VISIBILITY_GRAPH_DEBUG
       std::cout << "insert of edge from " << edge.first << " to " 
                 << edge.second 
       if (result.second)
          std::cout << " was successful " << std::endl;
       else
          std::cout << " was NOT successful " << std::endl;
#endif
   }

   bool is_an_edge(const Point_pair& edge)
   {
      if (less_xy_2(edge.first,edge.second))
         return edges.find(edge) != edges.end();
      else 
         return edges.find(Point_pair(edge.second, edge.first)) != edges.end();
   }

// ??? need to finish this ???
   template <class ForwardIterator>
   bool is_valid(ForwardIterator first, ForwardIterator beyond);

private:

   void print_vertex_map(const Vertex_map& vertex_map, const Polygon_2& polygon)
   {
      typedef typename Vertex_map::const_iterator    const_iterator;

      for (const_iterator it = vertex_map.begin(); it != vertex_map.end(); it++)
      {
         if ((*it).second.second != polygon.end())
         std::cout << (*it).first << " sees " << *((*it).second.second) 
                   << std::endl;
      }
   }

   template<class E>
   void print_edge_set(const E& edges)
   {
      typedef typename E::iterator   iterator;
      for (iterator it = edges.begin(); it != edges.end(); it++)
      {
         std::cout << (*it).first << " " << (*it).second << std::endl;
      }
   }

   // want to determine, for each vertex p of the polygon, the line segment
   // immediately below it.  For vertical edges, the segment below is not the
   // one that begins at the other endpoint of the edge.
   void initialize_vertex_map(const Polygon_2& polygon, Vertex_map& vertex_map);

   // determines if one makes a left turn going from p to q to q's parent.
   // if q's parent is p_infinity, then a left turn is made when p's x value
   // is less than q's x value or the x values are the same and p's y value is
   // less than q's.
   // if p, q, and q's parent are collinear, then one makes a "left turn"
   // if q is between p and q's parent (since this means that p can't see 
   // q's parent and thus should not become a child of that node)
   bool left_turn_to_parent(Tree_iterator p, Tree_iterator q, Tree& tree);


   // returns true if q is the vertex after p
   bool is_next_to(const Polygon_2& polygon, Polygon_const_iterator p, 
                   Polygon_const_iterator q) const
   {
      Polygon_const_iterator next = p; next++;
      if (next == polygon.end()) next = polygon.begin();
      return (q == next);
   }

   // returns true if q is the vertex before or after p
   bool are_adjacent(const Polygon_2& polygon, Polygon_const_iterator p, 
                     Polygon_const_iterator q) const
   {
      Polygon_const_iterator next = p; next++;
      if (next == polygon.end()) next = polygon.begin();
      if (q == next) return true;
      next = q; next++;
      if (next == polygon.end()) next = polygon.begin();
      if (p == next) return true;
      return false;
   }

   // returns true if the diagonal from p to q cuts the interior angle at p
   bool diagonal_in_interior(const Polygon_2& polygon, Polygon_const_iterator p,
                             Polygon_const_iterator q);
 

   // returns true if the looker can see the point_to_see 
   bool point_is_visible(const Polygon_2& polygon, 
                         Polygon_const_iterator point_to_see, 
                         Vertex_map_iterator looker);
   
   void update_visibility(Vertex_map_iterator p_it,
                          Vertex_map_iterator q_it, 
                          const Polygon_2& polygon, int are_adjacent);

   void update_collinear_visibility(Vertex_map_iterator p_it,
                                    Vertex_map_iterator q_it, 
                                    const Polygon_2& polygon);

   // The segment between points p and q is a potential visibility edge
   // This function determines if the edge should be added or not (based
   // on p's current visibility point) and updates p's visibility point
   // where appropriate
   void handle(Tree_iterator p, Tree_iterator q, const Polygon_2& polygon,
               Vertex_map& vertex_map);

private:
   Leftturn_2                            leftturn_2;
   Orientation_2                         orientation_2;
   Collinear_are_ordered_along_line_2    collinear_ordered_2;
   Are_strictly_ordered_along_line_2     are_strictly_ordered_along_line_2;
   Less_xy_2                             less_xy_2;
   Construct_segment_2                   construct_segment_2;
   Construct_ray_2                       construct_ray_2;
   Edge_set                              edges;
};

}

#ifdef CGAL_CFG_NO_AUTOMATIC_TEMPLATE_INCLUSION
#include <CGAL/Vertex_visibility_graph_2.C>
#endif // CGAL_CFG_NO_AUTOMATIC_TEMPLATE_INCLUSION

#endif // CGAL_VERTEX_VISIBILITY_GRAPH_2_H
