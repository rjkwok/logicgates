EXE_NAME = debug

OPTIONS = \
	-static-libstdc++ \
	-static-libgcc \
	-mwindows

SFML_LIBS = \
	-lsfml-graphics-s \
	-lsfml-window-s \
	-lsfml-system-s

SFML_DEP_LIBS = \
	-lopengl32 \
	-lwinmm \
	-lgdi32 \
	-lFLAC \
	-lfreetype \
	-ljpeg \
	-logg \
	-lopenal32 \
	-lvorbis \
	-lvorbisenc \
	-lvorbisfile

all: clean
	g++ -std=c++11 $(OPTIONS) -Iinclude -Llib -D SFML_STATIC src/*.cpp -o $(EXE_NAME) $(SFML_LIBS) $(SFML_DEP_LIBS)

clean:
	-rm $(EXE_NAME).exe