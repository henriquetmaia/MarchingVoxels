// -----------------------------------------------------------------------------
// libNITRO -- Space.h
// -----------------------------------------------------------------------------
//
// Position Based control of mesh interactions and dynamics
// see [Position Based Constraints - mueller et al 2007] for details
//

#ifndef NITRO_SPACE_H
#define NITRO_SPACE_H

#include "Vector.h"
#include "Real.h"
#include "Mesh.h"

using namespace std;

namespace NITRO{

   struct Sample
   {
      Sample( Vector v, Real r ): coordinate(v), density(r){}
      ~Sample();
      Vector coordinate;
      Real density;
   };

   struct DensityField
   {
      vector< Sample* > samples;
      Mesh* tesselation;
   };

   class Space
   {
   public:
      Space( void );
      Space( const unsigned x_dim,
             const unsigned y_dim,
             const unsigned z_dim,
             const double density );

      ~Space( void );
      
      int addField( const string& filename );

      void march( void );

      Mesh* operator[] ( const unsigned& index );
      // returns reference to the specified mesh (0-based indexing)

      const Mesh* operator[] ( const unsigned& index ) const;
      // returns const reference to the specified mesh (0-based indexing)         

      const unsigned size( void );
      // returns number of fields contained in this space

      vector< DensityField* > m_sampleSets;

   private:
      unsigned int m_fieldCount;
      void normalize( void );
      void spatiallyHash( void );
      unsigned voxelHash( unsigned N, Vector coord, unsigned dim );
      void marchCube( unsigned i, unsigned j, unsigned k );
      void readSample( stringstream& ss );
      void updateGrid( const Vector newSample );
      unsigned m_grid[3];
      double m_density; 
      Vector min_coord;
      Vector max_coord;

      vector< Sample* >* voxelVertices;
      // unordered_map< Vector, vector< Sample * > > voxels;

   };

}
#endif
