
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


all: 	$(objs)
		c++ -I./amigaos VectorPlay.cpp  $(objs) -o main.exe -lauto -lvorbisfile -lvorbis -logg -lalut -lopenal -lpthread  -lpng -lmpg123 -lz  -lm -Wno-write-strings

Debug/settings.obj: settings.cpp
		c++ -I./amigaos -c settings.cpp -o Debug/settings.obj

Debug/PMap.obj: PMap.cpp
		c++ -I./amigaos -c PMap.cpp -o Debug/PMap.obj

Debug/player.obj: player.cpp
		c++ -I./amigaos -c player.cpp -o Debug/player.obj

Debug/player_kneeling.obj: player_kneeling.cpp
		c++ -I./amigaos -c player_kneeling.cpp -o Debug/player_kneeling.obj

Debug/player_climbing.obj: player_climbing.cpp
		c++ -I./amigaos -c player_climbing.cpp -o Debug/player_climbing.obj

Debug/player_jumping.obj: player_jumping.cpp
		c++ -I./amigaos -c player_jumping.cpp -o Debug/player_jumping.obj

Debug/enemy_platform.obj: enemy_platform.cpp
		c++ -I./amigaos -c enemy_platform.cpp -o Debug/enemy_platform.obj

Debug/enemy.obj: enemy.cpp 
		c++ -I./amigaos -c enemy.cpp -o Debug/enemy.obj

Debug/spawn.obj: spawn.cpp
		c++ -I./amigaos -c spawn.cpp -o Debug/spawn.obj

Debug/bullet.obj: bullet.cpp
		c++ -I./amigaos -c bullet.cpp -o Debug/bullet.obj

Debug/Gfx.obj: Gfx.cpp
		c++ -I./amigaos -c Gfx.cpp -o Debug/Gfx.obj

Debug/ObjAnim.obj: ObjAnim.cpp ObjAnim.h
		c++ -I./amigaos -c ObjAnim.cpp -o Debug/ObjAnim.obj

Debug/Sound.obj: Sound.cpp
		c++ -I./amigaos -c Sound.cpp -o Debug/Sound.obj

Debug/allegro5.obj: amigaos/allegro5/allegro.cpp
		c++ -I./amigaos -c amigaos/allegro5/allegro.cpp -o Debug/allegro5.obj

Debug/allegro5_sound.obj: amigaos/allegro5/allegro_sound.cpp 
		c++  -I./amigaos -c amigaos/allegro5/allegro_sound.cpp -o Debug/allegro5_sound.obj -Iamigaos

Debug/%.obj: %.cpp
		c++	-I./amigaos $< -o $@ -c

clean:
		delete Debug/#?.obj
