#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#include "Viewer.h"
#include "Image.h"

namespace NITRO
{
   // declare static member variables
   Viewer::RenderMode Viewer::mode = renderShaded;
   GLuint Viewer::surfaceDL = 0;
   int Viewer::windowSize[2] = { 512, 512 };
   Camera Viewer::camera;
   Shader Viewer::shader;
   
   void Viewer :: init( void )
   {
      restoreViewerState();
      initGLUT();
      initGL();
      initGLSL();
   
      updateDisplayList();
   
      glutMainLoop();
   }

   void Viewer :: initGL( void )
   {
      glClearColor( .5, .5, .5, 1. );
   }
   
   void Viewer :: initGLUT( void )
   {
      int argc = 0;
      vector< vector<char> > argv(1);
   
      // initialize window
      glutInitWindowSize( windowSize[0], windowSize[1] );
      glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
      glutInit( &argc, (char**)&argv );
      glutCreateWindow( "MarchingCubes - htm2104" );
   
      // specify callbacks
      glutDisplayFunc  ( Viewer::display  );
      glutIdleFunc     ( Viewer::idle     );
      glutKeyboardFunc ( Viewer::keyboard );
      glutSpecialFunc  ( Viewer::special  );
      glutMouseFunc    ( Viewer::mouse    );
      glutMotionFunc   ( Viewer::motion   );
   
      // initialize menus
      int viewMenu = glutCreateMenu( Viewer::view );
      glutSetMenu( viewMenu );
      glutAddMenuEntry( "[s] Smooth Shaded",  menuSmoothShaded );
      glutAddMenuEntry( "[f] Wireframe",      menuWireframe    );
      glutAddMenuEntry( "[↑] Zoom In",    menuZoomIn       );
      glutAddMenuEntry( "[↓] Zoom Out",   menuZoomOut      );

      int mainMenu = glutCreateMenu( Viewer::menu );
      glutSetMenu( mainMenu );
      glutAddMenuEntry( "[space] Process Mesh", menuProcess    );
      glutAddMenuEntry( "[r] Reset Mesh",       menuResetMesh  );
      glutAddMenuEntry( "[w] Write Mesh",       menuWriteMesh  );
      glutAddMenuEntry( "[\\] Screenshot",      menuScreenshot );
      glutAddMenuEntry( "[esc] Exit",           menuExit       );
      glutAddSubMenu( "View", viewMenu );
      glutAttachMenu( GLUT_RIGHT_BUTTON );
   }

   void Viewer :: initGLSL( void )
   {
      shader.loadVertex( "shaders/vertex.glsl" ); // Change these to Toon-shaders
      shader.loadFragment( "shaders/fragment.glsl" ); // or Cel-shaders
   }
   
   void Viewer :: menu( int value )
   {
      switch( value )
      {
         case( menuProcess ):
            mProcess();
            break;
         case( menuResetMesh ):
            mResetMesh();
            break;
         case( menuWriteMesh ):
            mWriteMesh();
            break;
         case( menuScreenshot ):
            mScreenshot();
            break;
         case( menuExit ):
            mExit();
            break;
         default:
            break;
      }
   }
   
   void Viewer :: view( int value )
   {
      switch( value )
      {
         case( menuSmoothShaded ):
            mSmoothShaded();
            break;
         case( menuWireframe ):
            mWireframe();
            break;
         case( menuZoomIn ):
            mZoomIn();
            break;
         case( menuZoomOut ):
            mZoomOut();
            break;
         default:
            break;
      }
   }
   
   void Viewer :: mProcess( void )
   {

         // static double sim_time = 0.;
         // timing information for profiling

         if( fields.size() == 0 ){
            cerr << "[Viewer::mProcess] No DensityFields to process" << endl;
            exit( EXIT_FAILURE );
         }
         // for every densityfield
         // cout << "Finished MarchingCubes on densityfield["<<d <<"], in d_time: " << sim_time << endl;

         updateDisplayList();
   }
   
   void Viewer :: mResetMesh( void )
   {

      // use this later to reset boolean operations done to the original mesh
      // for( unsigned m = 0; m < sim.size(); ++m ){
      //    sim[m]->reload();         
      // }
      updateDisplayList();
   }
   
   void Viewer :: mWriteMesh( void )
   {
      // use this later to save out meshes edited by boolean operations
      // for( unsigned m = 0; m < sim.size(); ++m ){
      //    sim[m]->write( "out" + to_string( m ) + ".obj" );         
      // }
   }
   
   void Viewer :: mExit( void )
   {
      storeViewerState();
      exit( 0 );
   }
   
   void Viewer :: mSmoothShaded( void )
   {
      mode = renderShaded;
      updateDisplayList();
   }
   
   void Viewer :: mWireframe( void )
   {
      mode = renderWireframe;
      updateDisplayList();
   }

   void Viewer :: mZoomIn( void )
   {
      camera.zoomIn();
   }

   void Viewer :: mZoomOut( void )
   {
      camera.zoomOut();
   }

   void Viewer :: mScreenshot( void )
   {
      static int index = 0;
   
      // get window width and height
      GLint view[4];
      glGetIntegerv( GL_VIEWPORT, view );
      int w = view[2];
      int h = view[3];
   
      // get pixels
      Image image( w, h );
      glReadPixels( 0, 0, w, h, GL_BGR, GL_FLOAT, &image(0,0) );
   
      stringstream filename;
      filename << "frames/viewer" << setw(8) << setfill( '0' ) << index << ".tga";
      image.write( filename.str().c_str() );
   
      index++;
   }
   
   void Viewer :: keyboard( unsigned char c, int x, int y )
   {
      switch( c )
      {
         case 's':
            mSmoothShaded();
            break;
         case 'f':
            mWireframe();
            break;
         case 'w':
            mWriteMesh();
            break;
         case 'r':
            mResetMesh();
            break;
         case '\\':
            mScreenshot();
            break;
         case ' ':
            mProcess();
            break;
         case 27:
            mExit();
            break;
         default:
            break;
      }
   }

   void Viewer :: special( int i, int x, int y )
   {
      switch( i )
      {
         case GLUT_KEY_UP:
            camera.zoomIn();
            break;
         case GLUT_KEY_DOWN:
            camera.zoomOut();
            break;
         case 27:
            mExit();
            break;
         default:
            break;
      }
   }
   
   void Viewer :: display( void )
   {
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      shader.enable();
   
      glMatrixMode( GL_PROJECTION );
      glLoadIdentity();
   
      GLint viewport[4];
      glGetIntegerv( GL_VIEWPORT, viewport );
      double aspect = (double) viewport[2] / (double) viewport[3];
      const double fovy = 50.;
      const double clipNear = .01;
      const double clipFar = 1000.;
      gluPerspective( fovy, aspect, clipNear, clipFar );
   
      glMatrixMode( GL_MODELVIEW );
      glLoadIdentity();

      Quaternion    eye = Vector( 0., 0., -2.5*camera.zoom );
      Quaternion center = Vector( 0., 0., 0. );
      Quaternion     up = Vector( 0., 1., 0. );

      gluLookAt(    eye[1],    eye[2],    eye[3],
                 center[1], center[2], center[3],
                     up[1],     up[2],     up[3] );


      Quaternion r = camera.currentRotation();

      eye = r.conj() * eye * r;
      GLint uniformEye = glGetUniformLocation( shader, "eye" );
      glUniform3f( uniformEye, eye[1], eye[2], eye[3] );

      Quaternion light = Vector( -1., 1., -2. );
      light = r.conj() * light * r;
      GLint uniformLight = glGetUniformLocation( shader, "light" );
      glUniform3f( uniformLight, light[1], light[2], light[3] );

      camera.setView();
   
      drawSurface();

      shader.disable();
   
      glutSwapBuffers();
   }

   void Viewer :: drawSurface( void )
   {
      glPushAttrib( GL_ALL_ATTRIB_BITS );

      glEnable( GL_DEPTH_TEST );
      glEnable( GL_LIGHTING );
   
      glCallList( surfaceDL );
   
      glPopAttrib();
   }
   
   void Viewer :: drawMesh( void )
   {
      glPushAttrib( GL_ALL_ATTRIB_BITS );

      glEnable( GL_POLYGON_OFFSET_FILL );
      glPolygonOffset( 1., 1. );
   
      drawPolygons();
   
      glDisable( GL_POLYGON_OFFSET_FILL );
   
      if( mode == renderWireframe )
      {
         shader.disable();
         drawWireframe();
      }

      drawIsolatedVertices();

      glPopAttrib();
   }

   void Viewer :: drawPolygons( void )
   {
      // glColor3f( 1., .4, 0. ); // Caltech Orange
      glColor3f( 0.549, 0.839, 1. ); // Columbia Blue

      for( unsigned m = 0; m < sim.size(); ++m ){
         for( FaceCIter f  = sim[m]->faces.begin();
                        f != sim[m]->faces.end();
                        f ++ )
         {
            if( f->isBoundary() ) continue;

            glBegin( GL_POLYGON );
            if( mode == renderWireframe )
            {
               Vector N = f->normal();
               glNormal3dv( &N[0] );
            }

            HalfEdgeCIter he = f->he;
            do
            {
               if( mode != renderWireframe )
               {
                  Vector N = he->vertex->normal();
                  glNormal3dv( &N[0] );
               }

               glVertex3dv( &he->vertex->position[0] );

               he = he->next;
            }
            while( he != f->he );
            glEnd();
         }
      }
   }

   void Viewer :: drawWireframe( void )
   {
      glPushAttrib( GL_ALL_ATTRIB_BITS );

      glDisable( GL_LIGHTING );
      glColor4f( 0., 0., 0., .5 );
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

      glBegin( GL_LINES );
      for( unsigned m = 0; m < sim.size(); ++m ){
         for( EdgeCIter e  = sim[m]->edges.begin();
               e != sim[m]->edges.end();
               e ++ )
         {
            glVertex3dv( &e->he->vertex->position[0] );
            glVertex3dv( &e->he->flip->vertex->position[0] );
         }
      }
      glEnd();

      glPopAttrib();
   }

   void Viewer :: drawIsolatedVertices( void )
   {
      glPushAttrib( GL_ALL_ATTRIB_BITS );

      // draw with big, round, red dots
      glPointSize( 5 );
      glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
      glEnable( GL_POINT_SMOOTH );
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      glColor4f( 1., 0., 0., 1. ); // red

      glBegin( GL_POINTS );
      for( unsigned m = 0; m < sim.size(); ++m ){
         for( VertexCIter v  = sim[m]->vertices.begin();
                          v != sim[m]->vertices.end();
                          v ++ )
         {
            if( v->isIsolated() )
            {
               glVertex3dv( &v->position[0] );
            }
         }
      }
      glEnd();

      glPopAttrib();
   }
   
   void Viewer :: updateDisplayList( void )
   {
      if( surfaceDL )
      {
         glDeleteLists( surfaceDL, 1 );
         surfaceDL = 0;
      }
   
      surfaceDL = glGenLists( 1 );
   
      glNewList( surfaceDL, GL_COMPILE );
      drawMesh();
      glEndList();
   }
   
   void Viewer :: mouse( int button, int state, int x, int y )
   {
      camera.mouse( button, state, x, y );
   }

   void Viewer :: motion( int x, int y )
   {
      camera.motion( x, y );
   }
   
   void Viewer :: idle( void )
   {
      camera.idle();
      mProcess();
      glutPostRedisplay();
   }

   void Viewer :: storeViewerState( void )
   {
      ofstream out( ".viewer_state.txt" );

      out << camera.rLast[0] << endl;
      out << camera.rLast[1] << endl;
      out << camera.rLast[2] << endl;
      out << camera.rLast[3] << endl;

      GLint view[4];
      glGetIntegerv( GL_VIEWPORT, view );
      out << view[2] << endl;
      out << view[3] << endl;

      out << (int) mode << endl;
   }

   void Viewer :: restoreViewerState( void )
   {
      ifstream in( ".viewer_state.txt" );

      if( !in.is_open() ) return;

      in >> camera.rLast[0];
      in >> camera.rLast[1];
      in >> camera.rLast[2];
      in >> camera.rLast[3];

      in >> windowSize[0];
      in >> windowSize[1];

      int m;
      in >> m;
      mode = (RenderMode) m;
   }
}

