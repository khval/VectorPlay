
objs =	Debug/allegro5.obj 	\
		Debug/allegro5_sound.obj \
		Debug/animation.obj \
		Debug/buttons.obj \
		Debug/debug.obj \
		Debug/intersect.obj \
		Debug/SelectFileAmigaOS.obj \
		Debug/SimpleXML.obj \
		Debug/stdafx.obj \
		Debug/vector.obj \

libs = -lauto -lvorbisfile -lvorbis -logg -lalut -lopenal -lpthread  -lpng -lmpg123 -lz  -lm

all: 	$(objs)
		c++ -I./amigaos VectorPlay.cpp  $(objs) -o main.exe $(libs) -Wno-write-strings

Debug/allegro5.obj: amigaos/allegro5/allegro.cpp
		c++ -I./amigaos -c amigaos/allegro5/allegro.cpp -o Debug/allegro5.obj

Debug/allegro5_sound.obj: amigaos/allegro5/allegro_sound.cpp 
		c++  -I./amigaos -c amigaos/allegro5/allegro_sound.cpp -o Debug/allegro5_sound.obj -Iamigaos

Debug/%.obj: %.cpp
		c++	-I./amigaos $< -o $@ -c

clean:
		delete Debug/#?.obj
