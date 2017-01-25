
#ifdef amigaos4
#include "main_amigaos4.h"
#else
#include "main_windows.h"
#endif


class Part
{
public:
	char *name;
	ALLEGRO_BITMAP *bitmap;
	xy start;
	xy end;
	xy init_hotspot;
	xy hotspot;
	float rad;
	void save(FILE *fd);
	Part(char *name, ALLEGRO_BITMAP *_bitmap, xy _start, xy _end, xy _hotspot, double _rad);
	~Part();
};

class Bone
{
public:
	ALLEGRO_COLOR color;
	int sort;
	char *name;
	char *connectedTo;
	Part *part;
	xy pos;
	void draw();
	Bone();
	void save(FILE *fd);
	void save_pos(FILE *fd);
};

class Frame
{
public:
	int boneCount;
	Bone **bones;

#ifndef liniar_movement
	xy control_points[e_bone_count * 2];
#endif
	void init(double x, double y);
	void draw( ALLEGRO_COLOR c );
	int get_picked_vector(double x, double y);
#ifndef liniar_movement
	void reinit_controll_points(Frame &before, Frame &after);
	void sort_controll_points(Frame &before, Frame &after);
#endif
	Frame( int BoneCount );
	~Frame();
};

class Animation
{
	private: 
		void Animation::__fix_bone_references(Bone **rootbones, Bone **bones);

	public:
	int frameCount;
	int boneCount;
	int partCount;
	Part **parts;
	Frame **frames;
	Frame *final;

	void setBitmap(ALLEGRO_BITMAP *bm);
	void sortBones();
	void transform_animation(double p, Frame &before, Frame &current, Frame &after);
	void copyBoneProperties();
	Part *findPart(char *name);
	Animation(int FrameCount, int PartCount, int BoneCount);
	~Animation();
};

class Creature
{
	public:
		double x;
		double y;

};
