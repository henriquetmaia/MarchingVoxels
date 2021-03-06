// -----------------------------------------------------------------------------
// libDDG -- Edge.h
// -----------------------------------------------------------------------------
//
// Edge stores attributes associated with a mesh edge.  The iterator he points
// to one of its two associated halfedges.  (See the documentation for a more
// in-depth discussion of the halfedge data structure.)
// 

#ifndef DDG_EDGE_H
#define DDG_EDGE_H

#include "Types.h"

namespace NITRO
{
   class Edge
   {
      public:
         HalfEdgeIter he;
         // points to one of the two halfedges associated with this edge
   };
}

#endif
