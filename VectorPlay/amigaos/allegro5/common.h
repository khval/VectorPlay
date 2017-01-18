
#ifndef __common_h__
#define __common_h__

#ifdef __amigaos4__
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/locale.h>
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>
#include <libraries/keymap.h>
#include <AL/al.h>
#include <AL/alut.h>
#else
// just to make virtual studio happy.
#define ULONG unsigned int
#define uint32 unsigned int
#define BOOL bool
#define ALuint unsigned int
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef ULONG ALLEGRO_COLOR;


enum ALLEGRO_PLAYMODE {
	ALLEGRO_PLAYMODE_ONCE,
	ALLEGRO_PLAYMODE_LOOP
};

enum {
	ALLEGRO_SUGGEST,
	ALLEGRO_VSYNC
};

typedef struct TextFont ALLEGRO_FONT;

typedef struct
{
	ULONG format;
	unsigned char* data;
	short bits;
	short channels;
	uint32 samplerate;
	uint32 byterate;
	int size;
	double duration;
	ALuint ALbuffer;
} ALLEGRO_SAMPLE;

typedef struct
{
	int voice;
	ALLEGRO_SAMPLE *sample;
	uint32 playmode;
	uint32 mixer;
} ALLEGRO_SAMPLE_INSTANCE;

typedef struct
{
	int voice;
	ALLEGRO_SAMPLE_INSTANCE *instance;
} ALLEGRO_MIXER;

typedef struct {
	struct Window *window;
	struct Screen *screen;
	struct BitMap *bitmap;
} ALLEGRO_DISPLAY;

typedef struct {
	BOOL open;
	struct MsgPort *mp;
	struct TimeRequest *tr;
} ALLEGRO_TIMER;

typedef struct {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_TIMER *timer;
	void *keyboard;
	void *mouse;
} ALLEGRO_EVENT_QUEUE;

typedef struct {
	int wtf;
	int type;
	struct { int x; int y; 	int button; } mouse;
	struct { int keycode; long long int unichar; } keyboard;
} ALLEGRO_EVENT;

typedef struct {
	int x;
	int y;
} ALLEGRO_MOUSE_STATE;

#endif
