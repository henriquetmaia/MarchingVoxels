
#include <fstream>
#include <iostream>
#include <sstream>
#include "Space.h"

using namespace std;
namespace NITRO
{

	Space :: Space( 
		const unsigned x_dim,
		const unsigned y_dim,
		const unsigned z_dim,
		const double density  )
	{
		m_fieldCount = 0;
		m_density = density;

		if( x_dim == 0 || y_dim == 0 || z_dim == 0 ){
			cerr << "voxels dimensiona must be at least 1 " << endl;
			exit( EXIT_FAILURE );
		}
		// these need to be >= 2, should check that somewhere
		m_grid[0] = x_dim + 1;
		m_grid[1] = y_dim + 1;
		m_grid[2] = z_dim + 1;
	}

	Space :: Space( void )
	{
		m_fieldCount = 0;
		m_density = 0.;
		m_grid[0] = 0;
		m_grid[1] = 0;
		m_grid[2] = 0;
	}

	Space :: ~Space( void )
	{}

	int Space :: addField( const string& filename ){

		ifstream in( filename.c_str() );
		if( !in.is_open() )
		{
			cerr << "Error reading from DensityField file " << filename << endl;
			return 1;
		}

		m_sampleSets.push_back( new DensityField() );

		string line;
		while( getline( in, line ))
		{
			stringstream ss( line );
			string token;
			ss >> token;

			if( token == "vd" ) { readSample( ss ); continue; } // vertex (coordinate) & density
			if( token[0] == '#' ) continue; // comment
			if( token == "" ) continue; // empty string

			cerr << "Error: does not appear to be a valid DensityField file!" << endl;
			cerr << "(Offending line: " << line << ")" << endl;
			return 1;
		}

		m_sampleSets[m_fieldCount]->tesselation = new Mesh();

		normalize(); cout << "Normalizing coordinates of DensityField " << m_fieldCount << endl;

		++m_fieldCount;

		spatiallyHash();

		return 0;
	}

	Mesh* Space :: operator[]( const unsigned& index )
	{
		return ( m_sampleSets )[ index ]->tesselation;
	}

	const Mesh* Space :: operator[]( const unsigned& index ) const
	{
		return ( m_sampleSets )[ index ]->tesselation;
	}

	const unsigned Space :: size( void ){
		return m_sampleSets.size();
	}


	void Space :: readSample( stringstream& ss )
	{
		double x, y, z, d;
		ss >> x >> y >> z >> d;

		m_sampleSets[m_fieldCount]->samples.push_back( new Sample( Vector( x, y, z ), d ) );
	}

   void Space :: normalize( void )
   {
      vector< Sample* > samples = m_sampleSets[m_fieldCount]->samples;
      int count = m_sampleSets[m_fieldCount]->samples.size();

      // compute center of mass
      Vector c( 0., 0., 0. );
      for( unsigned v = 0; v < count; ++v )
      {
         c += samples[v]->coordinate;
      }
      c /= (double) count;

      // translate to origin
      for( unsigned v = 0; v < count; ++v )
      {
         samples[v]->coordinate -= c;
      }

      // rescale such that the mesh sits inside the unit ball
      double rMax = 0.;
      for( unsigned v = 0; v < count; ++v )
      {
		rMax = max( rMax, samples[v]->coordinate.norm() );
      }
      for( unsigned v = 0; v < count; ++v )
      {
		samples[v]->coordinate /= rMax;
		updateGrid( samples[v]->coordinate );
      }

   }

   void Space :: spatiallyHash( void ){

   		// free( voxels );
   		// Flat[HEIGHT * WIDTH * DEPTH]
   		// Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
   		voxels = new vector< Sample* >[ m_grid[0] * m_grid[1] * m_grid[2] ];

   		for( unsigned f = 0; f < m_fieldCount; ++f ){

			vector< Sample* > samples = m_sampleSets[f]->samples;
			int count = m_sampleSets[f]->samples.size();

			for( unsigned s = 0; s < count; ++s )
			{

				unsigned xi = voxelHash( m_grid[0], samples[s]->coordinate, 0 );
				unsigned yi = voxelHash( m_grid[1], samples[s]->coordinate, 1 );
				unsigned zi = voxelHash( m_grid[2], samples[s]->coordinate, 2 );
				// cout << "adding to bin: [" << xi << "][" << yi << "][" << zi << "]" << endl;
				voxels[ xi + m_grid[1] * ( yi + m_grid[2] * zi )].push_back( samples[s] );
			}			

   		}
   }

   void Space :: march( void ){
   	
   }

   unsigned Space :: voxelHash( unsigned N, Vector coord, unsigned dim ){

   		double h = 2 / N;
   		double l = -1.0 - h/2;
   		double r =  1.0 + h/2;
		double alpha = ( coord[dim] - l ) / ( r - l );
		return  (unsigned) (alpha * (N ) );
   }

   void Space :: updateGrid( const Vector newSample ){
   	if( newSample.x < min_coord.x ) min_coord.x = newSample.x;
   	if( newSample.y < min_coord.y ) min_coord.y = newSample.y;
   	if( newSample.z < min_coord.z ) min_coord.z = newSample.z;

   	if( newSample.x > max_coord.x ) max_coord.x = newSample.x;
   	if( newSample.y > max_coord.y ) max_coord.y = newSample.y;
   	if( newSample.z > max_coord.z ) max_coord.z = newSample.z;

   }

}
