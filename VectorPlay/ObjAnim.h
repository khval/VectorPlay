#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/bitmap.h>
#include <list>
#include "main.h"

#pragma once

class Collision
{
public:
	int type;
	struct rect area;
	double scale;
	struct xy offset;
	void isInside(struct xy pos);
	bool mouseIsInside(struct xy pos, int &out_corner);
	void ScaleFromTopLeft(struct xy pos);
	void ScaleFromBottomRight(struct xy pos);
	void moveToMouse(struct xy pos);
	void draw();
	Collision& operator = (struct rect &other);

	Collision();
};

struct SpriteLine
{
	double startX; double endX;
} ;

class FrameData
{
public:
		FrameData(double width, double height);
		double top;
		double bottom;
		struct SpriteLine lines[7];
		void save(FILE *fd);
		void load(struct xy offset, FILE *fd);
};


class ObjAnim
{

	ALLEGRO_BITMAP *gfx;

public:
	
	int fx;
	int fy;
	int maxFrame;

	int TimeDelay;
	int animColumns;
	void (*animMode) (ObjAnim &anim,int &curFrame,int &animPingPongDir, int direction);
	int w;
	int h;
	int l_or_r;

	FrameData **frameData;

	void Load(const char *name, int animColumns, int MAX, int DELAY);
	void Unload();
	void Draw(int fx, int fy, int curFrame, int l_or_r);
	void DrawSpecial(int x, int y, int curFrame, double scale);
	void NextFrame(int &TimeCount, int &curFrame, int &animPingPongDir);
	void LastFrame(int &TimeCount, int &curFrame, int &animPingPongDir);
	
	ObjAnim( void (*animMode) (ObjAnim &anim,int &curFrame,int &animPingPongDir, int direction) );
	~ObjAnim();
};

void animOnce (ObjAnim &anim,int &curFrame, int &animPingPongDir,int direction);
void animLoop (ObjAnim &anim,int &curFrame, int &animPingPongDir,int direction);
void animPingPong (ObjAnim &anim,int &curFrame, int &animPingPongDir,int direction);

class CObj
{

public:
	ObjAnim *anim;
	int animPingPongDir;
	double x;
	double y;
	double velX;
	double velY;
	double dropspeed;
	double dirX;
	double dirY;
	double speed;
	int l_or_r;
	int id;
	int energy;
	int hitPoints;

	bool collision_bottom;
	bool collision;
	int on_block;
	int collision_key_timeout;

	int curFrame;
	int TimeCount;

	void *user_data;

	bool getMinMaxX(double deltaY, double &startX, double &endX);
	int w();
	int h();

	void(*move_fn) (CObj &me);
	void(*collision_fn) ( CObj &me);
	void(*draw_fn) ( CObj &me);
	bool(*inside_fn) ( CObj &me, struct xy &pos );
	void(*control_fn) (CObj &player, int keycode, int mode, int left, int right, int up, int down);

	CObj();
	CObj( CObj &other);

	void SetAnim(ObjAnim *anim);
	bool CheckAnim(ObjAnim *_anim);
	bool IsInside( struct xy &pos );	// we don't wont copy pos, just ref.

	void CheckObjCollision( void(**table) (CObj *self, CObj *other), CObj *other);
	void CheckMapCollision( void(**table) (CObj *self), bool(*check_map) (struct xy &pos));

	void NextFrame();
	void LastFrame();
	void Draw();
	void Move();

	~CObj();
};

/*
struct xy_col
{
	double x;
	double y;
	void(*col_fn)(CObj &me, int x, int y, CObj *other);
};
*/
