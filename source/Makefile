AR=ar
AS=as
CC=gcc
CPP=cpp
CXX=g++
LD=ld
OBJCOPY=objcopy
OBJDUMP=objdump
STRIP=strip
LDFLAGS=`pkg-config --libs gtk+-2.0 sndfile fluidsynth` -lgdk_imlib -lpng -lportaudio
CPPFLAGS=`pkg-config --cflags gtk+-2.0 sndfile fluidsynth` -Wall -ggdb -D_GLIBCXX_DEBUG
OBJDIR= ../obj
BINDIR= ../bin
OBJS= $(OBJDIR)/main.o $(OBJDIR)/GraphicTileset.o $(OBJDIR)/FontTileset.o $(OBJDIR)/TerrainMap.o $(OBJDIR)/WallShape.o $(OBJDIR)/SoundClip.o $(OBJDIR)/SoundLibraryMixer.o

all: $(BINDIR)/thegame

$(BINDIR)/thegame: $(OBJS)
	$(CXX) -o $(BINDIR)/thegame $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS)
	
$(OBJDIR)/main.o: main.cpp TerrainMap.h GraphicTileset.h FontTileset.h WallShape.h SoundLibraryMixer.h SoundClip.h
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) main.cpp -o $(OBJDIR)/main.o

$(OBJDIR)/GraphicTileset.o: GraphicTileset.cpp GraphicTileset.h
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) GraphicTileset.cpp -o $(OBJDIR)/GraphicTileset.o

$(OBJDIR)/FontTileset.o: FontTileset.cpp FontTileset.h GraphicTileset.h
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) FontTileset.cpp -o $(OBJDIR)/FontTileset.o

$(OBJDIR)/TerrainMap.o: TerrainMap.cpp TerrainMap.h GraphicTileset.h
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) TerrainMap.cpp -o $(OBJDIR)/TerrainMap.o

$(OBJDIR)/WallShape.o: WallShape.cpp WallShape.h 
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) WallShape.cpp -o $(OBJDIR)/WallShape.o

$(OBJDIR)/SoundClip.o: SoundClip.cpp SoundClip.h 
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) SoundClip.cpp -o $(OBJDIR)/SoundClip.o

$(OBJDIR)/SoundLibraryMixer.o: SoundLibraryMixer.cpp SoundLibraryMixer.h SoundClip.h 
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) SoundLibraryMixer.cpp -o $(OBJDIR)/SoundLibraryMixer.o

	
clean::
	-rm $(OBJS)
	
.PHONY: clean