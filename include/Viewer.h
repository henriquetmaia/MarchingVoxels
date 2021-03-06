// -----------------------------------------------------------------------------
// libDDG -- Viewer.h
// -----------------------------------------------------------------------------
//
// Viewer provides a graphical user interface (GUI) for inspecting and
// interacting with objects.  Viewer methods are static in order
// to make them compatible with GLUT callbacks.
//

#ifndef DDG_VIEWER_H
#define DDG_VIEWER_H

#include <GLUT/glut.h>

#include "Camera.h"
#include "Shader.h"
#include "Space.h"
#include "Vector.h"
#include "Real.h"

using namespace std;

namespace NITRO
{

   class Viewer
   {
      public:
         static void init( void );
         // displays the viewer until the program ends

         static Space fields;
         // contains all the density fields visualized by Viewer

      protected:
         // draw routines
         static void initGL( void );
         static void initGLUT( void );
         static void initGLSL( void );
         static void drawSurface( void );
         static void drawMesh( void );
         static void updateDisplayList( void );
         static void drawPolygons( void );
         static void drawWireframe( void );
         static void drawVertices( void );
         static void drawIsolatedVertices( void );
         
         static void changeResolution( bool higher );

         // GLUT callbacks
         static void display( void );
         static void idle( void );
         static void keyboard( unsigned char c, int x, int y );
         static void special( int i, int x, int y );
         static void mouse( int button, int state, int x, int y );
         static void motion( int x, int y );
         static void menu( int value );
         static void view( int value );
         
         // menu functions
         static void mProcess( void );
         static void mResetMesh( void );
         static void mWriteMesh( void );
         static void mExit( void );
         static void mSmoothShaded( void );
         static void mTextured( void );
         static void mWireframe( void );
         static void mZoomIn( void );
         static void mZoomOut( void );
         static void mScreenshot( void );

         // unique identifiers for menus
         enum
         {
            menuProcess,
            menuResetMesh,
            menuWriteMesh,
            menuExit,
            menuSmoothShaded,
            menuWireframe,
            menuZoomIn,
            menuZoomOut,
            menuScreenshot
         };

         // draw state
         enum RenderMode
         {
            renderShaded = 0,
            renderWireframe = 1
         };

         static RenderMode mode;
         // current render mode

         static void storeViewerState( void );
         static void restoreViewerState( void );
         static int windowSize[2];

         static Camera camera;
         // keeps track of view state

         static GLuint surfaceDL;
         // display list for mesh
         
         static Shader shader;
         // shader used to determine appearance of surface

   };
}

#endif

