// -----------------------------------------------------------------------------
// libDDG -- Vertex.h
// -----------------------------------------------------------------------------
//
// Vertex stores attributes associated with a mesh edge.  The iterator he
// points to its "outgoing" halfedge.  (See the documentation for a more
// in-depth discussion of the halfedge data structure.)
// 

#ifndef DDG_VERTEX_H
#define DDG_VERTEX_H

#include "Vector.h"
#include "Types.h"

namespace NITRO
{
   class Vertex
   {
      public:
         HalfEdgeIter he;
         // points to the "outgoing" halfedge

         Vector position;
         // location of vertex in Euclidean 3-space

         Vector normal( void ) const;
         // returns the vertex normal

         bool isIsolated( void ) const;
         // returns true if the vertex is not contained in any face or edge; false otherwise

         int valence( void ) const;
         // returns the number of incident faces / edges

         double dualArea( void ) const;
         // one third the area of incident faces, area of dual polygon surrounding vertex

   };
}

#endif

