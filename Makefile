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

OBJS = obj/main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

obj/main.o: src/main.cpp 
	$(CC) $(CFLAGS) -c src/main.cpp -o obj/main.o


clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -f $(TARGET).exe

