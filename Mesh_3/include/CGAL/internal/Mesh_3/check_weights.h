// Copyright (c) 2015 GeometryFactory (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: https://scm.gforge.inria.fr/svn/cgal/branches/features/Mesh_3-experimental-GF/Mesh_3/include/CGAL/internal/Mesh_3/get_index.h $
// $Id: get_index.h 67573 2012-02-02 14:54:51Z lrineau $
//
//
// Author(s)     : Jane Tournois
//
//******************************************************************************
// File Description :
//
//
//******************************************************************************

#ifndef CGAL_INTERNAL_MESH_3_CHECK_WEIGHTS_H
#define CGAL_INTERNAL_MESH_3_CHECK_WEIGHTS_H

#include <CGAL/license/Mesh_3.h>

#include <CGAL/enum.h>
#include <CGAL/tags.h>
#include <CGAL/Mesh_3/Has_features.h>
#include <boost/type_traits/is_same.hpp>

namespace CGAL {
namespace Mesh_3 {
namespace internal {


template<typename Triangulation, typename MeshDomain>
bool has_non_protecting_weights(const Triangulation& tr,
                                const MeshDomain&)
{
  bool with_features =
    boost::is_same<Has_features<MeshDomain>, CGAL::Tag_true>::value;

  typedef typename Triangulation::FT                FT;
  typedef typename Triangulation::Weighted_point    Weighted_point;

  typename Triangulation::Geom_traits::Compare_weighted_squared_radius_3 cwsr =
    tr.geom_traits().compare_weighted_squared_radius_3_object();

  for (typename Triangulation::Finite_vertices_iterator
        vv = tr.finite_vertices_begin();
        vv != tr.finite_vertices_end();
        ++vv)
  {
    const Weighted_point& vv_wp = tr.point(vv);
    if (cwsr(vv_wp, FT(0)) != CGAL::EQUAL)
    {
      if (with_features && vv->in_dimension() > 1)
        return true;
    }
  }
  return false;
}


} // end namespace internal
} // end namespace Mesh_3
} // end namespace CGAL

#endif //CGAL_INTERNAL_MESH_3_CHECK_WEIGHTS_H

