
#include <fstream>
#include <iostream>
#include <sstream>
#include "Space.h"
#include "March.h"

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

		if( x_dim <= 3 || y_dim <= 3 || z_dim <= 3 ){
			cerr << "voxels dimensiona must be at least 1 " << endl;
			exit( EXIT_FAILURE );
		}
		// these need to be >= 4, should check that somewhere
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
		for( unsigned v = 0; v < count; ++v ){
			c += samples[v]->coordinate;
		}
		c /= (double) count;

		// translate to origin
		for( unsigned v = 0; v < count; ++v ){
			samples[v]->coordinate -= c;
		}

		// rescale such that the mesh sits inside the unit ball
		double rMax = 0.;
		for( unsigned v = 0; v < count; ++v ){
			rMax = max( rMax, samples[v]->coordinate.norm() );
		}
		for( unsigned v = 0; v < count; ++v ){
			samples[v]->coordinate /= rMax;
			// updateGrid( samples[v]->coordinate );
		}
	}

	void Space :: spatiallyHash( void ){

   		// free( voxels );
   		// Flat[HEIGHT * WIDTH * DEPTH]
   		// Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
   		// I would prefer to have this be an unordered_map with my own hash and equivalence operator...
   		voxelVertices = new vector< Sample* >[ m_grid[0] * m_grid[1] * m_grid[2] ];

   		for( unsigned f = 0; f < m_fieldCount; ++f ){

			vector< Sample* > samples = m_sampleSets[f]->samples;
			int count = m_sampleSets[f]->samples.size();

			for( unsigned s = 0; s < count; ++s )
			{

				unsigned xi = voxelHash( m_grid[0], samples[s]->coordinate, 0 );
				unsigned yi = voxelHash( m_grid[1], samples[s]->coordinate, 1 );
				unsigned zi = voxelHash( m_grid[2], samples[s]->coordinate, 2 );
				// cout << "adding to bin: [" << xi << "][" << yi << "][" << zi << "]" << endl;
				voxelVertices[ xi + m_grid[1] * ( yi + m_grid[2] * zi )].push_back( samples[s] );
			}			

   		}
	}

	void Space :: march( void ){

   		// need to add support for different meshes with different density samples etc...
   		// for now just looking at the bin as a whole...
   		//		(i could assign a density field pointer inside Sample....)

   		// going to each voxel,
   		//	examine its vertices (interpolate the samples they contain, compare with Space's desired density)
   		//	extract a boolean value
   		//	determine what type of vertex-combination it is
   		//	assign appropriate edges
   		//	create triangles & normals, add them to the 


   		unsigned xBins = m_grid[0] - 2;
   		unsigned yBins = m_grid[1] - 2;
   		unsigned zBins = m_grid[2] - 2;

		for( unsigned k = 1; k < zBins; ++k ){
			// for each plane-slice of voxels
			for( unsigned j = 1; j < yBins; ++j ){
				for( unsigned i = 1; i < xBins; ++i ){
					// examine each voxel
					marchCube( i, j, k );
				}
			}
		}
	}

	void Space :: marchCube( unsigned i, unsigned j, unsigned k ){

   		double vertDensities[8];
   		Vector normals[8];
   		int8_t activeVerts = 0;
   		// Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
		for( unsigned z = 0; z < 2; ++z ){
			for( unsigned y = 0; y < 2; ++y ){
				for( unsigned x = 0; x < 2; ++x ){

					vector< Sample* > vert = voxelVertices[ (i + x) + m_grid[1] * ( (j + y) + m_grid[2] * (k + z) ) ];
					double vd = vertDensity( vert );

					if( vd >= m_density ){
						activeVerts |= 1 << ( x + 2 * (y + 2 * z) );          
					}
					vertDensities[ x + 2 * (y + 2 * z) ] = vd;

					normals[ x + 2 * (y + 2 * z) ] = getNormal( i + x, j + y, k + z );
				}
			}
		}


		int8_t edgesIntersected = cubeEdgeFlags[ activeVerts ];
		if( edgesIntersected == 0 ){
			return; // completely empty/full volume element
		}

		Vector baseVert = baseCoord( i, j, k );

		Vector edgeVertex[12];
		Vector vertNormal[12];
        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for( unsigned e = 0; e < 12; ++ e ){
            if( edgesIntersected & (1 << e) ){
            //if there is an intersection on this edge

                double offset = getOffset( vertDensities[ edgeConnection[e][0] ], vertDensities[ edgeConnection[e][1] ] );

                vertNormal[e] = (offset)*normals[ edgeConnection[e][0] ] + (1.0 - offset)*normals[ edgeConnection[e][1] ];

                edgeVertex[e].x = baseVert.x + (vertexOffset[ edgeConnection[e][0] ][0]  +  offset * edgeDirection[e][0]) * (2 / m_grid[0]);
                edgeVertex[e].y = baseVert.y + (vertexOffset[ edgeConnection[e][0] ][1]  +  offset * edgeDirection[e][1]) * (2 / m_grid[1]);
                edgeVertex[e].z = baseVert.z + (vertexOffset[ edgeConnection[e][0] ][2]  +  offset * edgeDirection[e][2]) * (2 / m_grid[2]);

            }
        }

        //Draw the triangles that were found.  There can be up to five per cube
        for( unsigned t = 0; t < 5; ++t ){
            if( triangleConnectionTable[ activeVerts ][ 3 * t ] < 0){
            	break;
            }

            for(unsigned c = 0; c < 3; ++c ){
                // int v = triangleConnectionTable[ activeVerts ][ 3 * t + c ];

                    // vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
                    // glColor3f(sColor.fX, sColor.fY, sColor.fZ);
                    // glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                    // glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
            }
        }
   }

   double Space :: vertDensity( vector< Sample* > samples ){

		if( unsigned numSamples = samples.size() != 0 ){
			double avgDensity = 0.;
			for( unsigned s = 0; s < numSamples; ++s ){
				avgDensity += samples[s]->density;
			}
			avgDensity /= numSamples;
			return avgDensity;
		}
   		return 0.;
   }

	// getoffset finds the approximate point of intersection of the surface
	// between two points with the values val1 and val2
	double Space :: getOffset( double value1, double value2 ){
        double delta = value2 - value1;
        if( delta == 0.0 ){
            return 0.5;
        }
        return ( m_density - value1 ) / delta;
	}

	// getNormal() finds the gradient of the scalar field at a point
	//This gradient can be used as a very accurate vertx normal for lighting calculations
	Vector Space :: getNormal( unsigned i, unsigned j, unsigned k ){

		Vector vertNormal;
		vertNormal.x = vertDensity( voxelVertices[ (i - 1) + m_grid[1] * ( j + m_grid[2] * k ) ] ) -
					   vertDensity( voxelVertices[ (i + 1) + m_grid[1] * ( j + m_grid[2] * k ) ] );
		vertNormal.x /= ( 2 / m_grid[0] );

		vertNormal.y = vertDensity( voxelVertices[ i + m_grid[1] * ( (j - 1) + m_grid[2] * k ) ] ) -
					   vertDensity( voxelVertices[ i + m_grid[1] * ( (j + 1) + m_grid[2] * k ) ] );
		vertNormal.y /= ( 2 / m_grid[1] );

		vertNormal.z = vertDensity( voxelVertices[ i + m_grid[1] * ( j + m_grid[2] * (k - 1) ) ] ) -
					   vertDensity( voxelVertices[ i + m_grid[1] * ( j + m_grid[2] * (k + 1) ) ] );
		vertNormal.z /= ( 2 / m_grid[2] );

		return vertNormal.unit();
	}


/*
{
	//vMarchCube1 performs the Marching Cubes algorithm on a single cube
GLvoid vMarchCube1(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fScale)
{
        extern GLint aiCubeEdgeFlags[256];
        extern GLint a2iTriangleConnectionTable[256][16];

        GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        GLfloat fOffset;
        GLvector sColor;
        GLfloat afCubeValue[8];
        GLvector asEdgeVertex[12];
        GLvector asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                afCubeValue[iVertex] = fSample(fX + a2fVertexOffset[iVertex][0]*fScale,
                                                   fY + a2fVertexOffset[iVertex][1]*fScale,
                                                   fZ + a2fVertexOffset[iVertex][2]*fScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= fTargetValue) 
                        iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0) 
        {
                return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {
                        fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], 
                                                     afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTargetValue);

                        asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
                        asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
                        asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

                        vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
                }
        }


        //Draw the triangles that were found.  There can be up to five per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
                if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
                        break;

                for(iCorner = 0; iCorner < 3; iCorner++)
                {
                        iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

                        vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
                        glColor3f(sColor.fX, sColor.fY, sColor.fZ);
                        glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                        glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
                }
        }
}
}
*/

	Vector Space :: baseCoord( unsigned i, unsigned j, unsigned k ){
  //  		double h = 2 / N;
  //  		double l = -1.0 - h/2;
		// double flr = l + h * i; 
		return Vector(
		( -1.0 - (1/ m_grid[0]) + (2/m_grid[0])*i ),
		( -1.0 - (1/ m_grid[1]) + (2/m_grid[1])*j ),
		( -1.0 - (1/ m_grid[2]) + (2/m_grid[2])*k )
					  );
	}


	unsigned Space :: voxelHash( unsigned N, Vector coord, unsigned dim ){

   		double h = 2 / N;
   		double l = -1.0 - h/2;
   		double r =  1.0 + h/2;
		double alpha = ( coord[dim] - l ) / ( r - l );
		return  (unsigned) (alpha * (N ) );
	}

	// void Space :: updateGrid( const Vector newSample ){
	//    	if( newSample.x < min_coord.x ) min_coord.x = newSample.x;
	//    	if( newSample.y < min_coord.y ) min_coord.y = newSample.y;
	//    	if( newSample.z < min_coord.z ) min_coord.z = newSample.z;

	//    	if( newSample.x > max_coord.x ) max_coord.x = newSample.x;
	//    	if( newSample.y > max_coord.y ) max_coord.y = newSample.y;
	//    	if( newSample.z > max_coord.z ) max_coord.z = newSample.z;
	// }

}
