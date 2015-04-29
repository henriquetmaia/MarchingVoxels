#######################################################################################################

# Specify library locations here (add or remove "#" marks to comment/uncomment lines for your platform)

# Mac OS X
NITRO_INCLUDE_PATH      = -I/usr/local/include/ -I/opt/local/include/
NITRO_LIBRARY_PATH      = -L/usr/local/lib/ -L/opt/local/lib/
NITRO_BLAS_LIBS         = -framework Accelerate
NITRO_SUITESPARSE_LIBS  = -lspqr -lumfpack -lcholmod -lmetis -lcolamd -lccolamd -lcamd -lamd -lm -lsuitesparseconfig
NITRO_OPENGL_LIBS       = -framework OpenGL -framework GLUT

# # Linux
# NITRO_INCLUDE_PATH      =
# NITRO_LIBRARY_PATH      =
# NITRO_BLAS_LIBS         = -llapack -lblas -lgfortran
# NITRO_SUITESPARSE_LIBS  = -lspqr -lcholmod -lmetis -lcolamd -lccolamd -lcamd -lamd -lm
# NITRO_OPENGL_LIBS       = -lglut -lGL -lGLU -lX11

# # Windows / Cygwin
# NITRO_INCLUDE_PATH      = -I/usr/include/opengl -I/usr/include/suitesparse
# NITRO_LIBRARY_PATH      = -L/usr/lib/w32api -L/usr/lib/suitesparse
# NITRO_BLAS_LIBS         = -llapack -lblas
# NITRO_SUITESPARSE_LIBS  = -lspqr -lcholmod -lcolamd -lccolamd -lcamd -lamd -lm
# NITRO_OPENGL_LIBS       = -lglut32 -lglu32 -lopengl32

#######################################################################################################

TARGET = marchingCubes
CC = g++
LD = g++
CFLAGS = -O3 -Wall -Wno-deprecated -Wno-unused-private-field -Werror -pedantic  $(NITRO_INCLUDE_PATH) -I./include -I./src -DNDEBUG
LFLAGS = -O3 -Wall -Wno-deprecated -Werror -pedantic $(NITRO_LIBRARY_PATH) -DNDEBUG
LIBS = $(NITRO_OPENGL_LIBS) $(NITRO_SUITESPARSE_LIBS) $(NITRO_BLAS_LIBS)

## !! Do not edit below this line -- dependencies can be updated by running ./update ##################

OBJS = obj/Camera.o obj/Complex.o obj/Edge.o obj/Face.o obj/HalfEdge.o obj/Image.o obj/Mesh.o obj/MeshIO.o obj/Quaternion.o obj/Real.o obj/Shader.o obj/Space.o obj/Vector.o obj/Vertex.o obj/Viewer.o obj/main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

obj/Camera.o: src/Camera.cpp include/Camera.h include/Quaternion.h include/Vector.h include/Complex.h 
	$(CC) $(CFLAGS) -c src/Camera.cpp -o obj/Camera.o

obj/Complex.o: src/Complex.cpp include/Complex.h 
	$(CC) $(CFLAGS) -c src/Complex.cpp -o obj/Complex.o

obj/Edge.o: src/Edge.cpp include/Edge.h include/Types.h include/Mesh.h include/HalfEdge.h include/Vector.h include/Vertex.h include/Face.h 
	$(CC) $(CFLAGS) -c src/Edge.cpp -o obj/Edge.o

obj/Face.o: src/Face.cpp include/Face.h include/Types.h include/Mesh.h include/HalfEdge.h include/Vector.h include/Vertex.h include/Edge.h 
	$(CC) $(CFLAGS) -c src/Face.cpp -o obj/Face.o

obj/HalfEdge.o: src/HalfEdge.cpp include/HalfEdge.h include/Vector.h include/Types.h include/Mesh.h include/Vertex.h include/Edge.h include/Face.h 
	$(CC) $(CFLAGS) -c src/HalfEdge.cpp -o obj/HalfEdge.o

obj/Image.o: src/Image.cpp include/Image.h 
	$(CC) $(CFLAGS) -c src/Image.cpp -o obj/Image.o

obj/Mesh.o: src/Mesh.cpp include/Mesh.h include/HalfEdge.h include/Vector.h include/Types.h include/Vertex.h include/Edge.h include/Face.h include/MeshIO.h 
	$(CC) $(CFLAGS) -c src/Mesh.cpp -o obj/Mesh.o

obj/MeshIO.o: src/MeshIO.cpp include/MeshIO.h include/Mesh.h include/HalfEdge.h include/Vector.h include/Types.h include/Vertex.h include/Edge.h include/Face.h 
	$(CC) $(CFLAGS) -c src/MeshIO.cpp -o obj/MeshIO.o

obj/Quaternion.o: src/Quaternion.cpp include/Quaternion.h include/Vector.h include/Complex.h 
	$(CC) $(CFLAGS) -c src/Quaternion.cpp -o obj/Quaternion.o

obj/Real.o: src/Real.cpp include/Real.h 
	$(CC) $(CFLAGS) -c src/Real.cpp -o obj/Real.o

obj/Shader.o: src/Shader.cpp include/Shader.h 
	$(CC) $(CFLAGS) -c src/Shader.cpp -o obj/Shader.o

obj/Space.o: src/Space.cpp include/Space.h include/Vector.h include/Real.h include/Mesh.h include/HalfEdge.h include/Types.h include/Vertex.h include/Edge.h include/Face.h include/March.h 
	$(CC) $(CFLAGS) -c src/Space.cpp -o obj/Space.o

obj/Vector.o: src/Vector.cpp include/Vector.h 
	$(CC) $(CFLAGS) -c src/Vector.cpp -o obj/Vector.o

obj/Vertex.o: src/Vertex.cpp include/Vertex.h include/Vector.h include/Types.h include/Mesh.h include/HalfEdge.h include/Edge.h include/Face.h 
	$(CC) $(CFLAGS) -c src/Vertex.cpp -o obj/Vertex.o

obj/Viewer.o: src/Viewer.cpp include/Viewer.h include/Camera.h include/Quaternion.h include/Vector.h include/Complex.h include/Shader.h include/Space.h include/Real.h include/Mesh.h include/HalfEdge.h include/Types.h include/Vertex.h include/Edge.h include/Face.h include/Image.h 
	$(CC) $(CFLAGS) -c src/Viewer.cpp -o obj/Viewer.o

obj/main.o: src/main.cpp include/Viewer.h include/Camera.h include/Quaternion.h include/Vector.h include/Complex.h include/Shader.h include/Space.h include/Real.h include/Mesh.h include/HalfEdge.h include/Types.h include/Vertex.h include/Edge.h include/Face.h 
	$(CC) $(CFLAGS) -c src/main.cpp -o obj/main.o


clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -f $(TARGET).exe

