
/*
Copyright 2017 Kjetil Hvalstrand (LiveForIt-Software.net)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "allegro5/allegro.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"
#include <allegro5/allegro_primitives.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "animation.h"
#include "intersect.h"

#if defined(amigaos4) || defined(linux)
#include <stdint.h>
#endif

//extern Part *parts[e_bone_count];

#ifdef editor
extern double zoom;
extern xy origo;
#endif

void Part::save(FILE *fd)
{

	fprintf(fd, "<part>\n");
	fprintf(fd, "<name>%s</name>\n",this->name ? this->name : "none" );
	fprintf(fd, "<hotspot><x>%0.2f</x><y>%0.2f</y></hotspot>\n", init_hotspot.rel_x, init_hotspot.rel_y);
	fprintf(fd, "<start><x>%0.2f</x><y>%0.2f</y></start>\n", this->start.x(), this->start.y());
	fprintf(fd, "<end><x>%0.2f</x><y>%0.2f</y></end>\n", this->end.x(), this->end.y());
	fprintf(fd, "<degrees>%0.2f</degrees>\n", (this->rad * 360.0f / (M_PI * 2)));
	fprintf(fd, "</part>\n");		
}


Part::Part(char *_name,ALLEGRO_BITMAP *_bitmap, xy _start, xy _end, xy _hotspot, double _rad)
{
	init_hotspot = _hotspot;
	name = strdup(_name);
	bitmap = _bitmap;
	start = _start;
	hotspot = - ((_start -_end) * _hotspot);
	end = _end;
	rad = _rad;
}

Part::~Part()
{
	if (name) free(name);
	name = NULL;
}

Bone::Bone()
{
	sort = 0;
	part = NULL;
	name = NULL;
	connectedTo = NULL;
	pos.set_angel_vector(0, 10);
}


void Bone::save( FILE *fd )
{
	unsigned char a, r, g, b;
	uint32_t argb;

	al_unmap_rgba(this->color, &r, &g, &b, &a);

	argb = (a << 24) + (r << 16) + (g << 8) + b ;

	fprintf(fd, "<bone><sort>%d</sort><name>%s</name><connectedTo>%s</connectedTo><color>0x%08X</color><min>%0.2lf</min><max>%0.2lf</max><part>%s</part></bone>\n",
		this->sort,
		this->name ? this->name : "NoName",
		this->connectedTo ? this->connectedTo : "none",
		argb,
		this->pos.length_min,
		this->pos.length_max,
		this->part ? this->part->name : "none");
}

void Bone::save_pos(FILE *fd)
{
	fprintf(fd, "<bone><x>%0.2f</x><y>%0.2f</y></bone>\n",  this->pos.rel_x , this-> pos.rel_y );
}


void Bone::draw( )
{
	double x, y;
	if (pos.ref)
	{
		if ((part) && (part -> bitmap))
		{

#ifdef editor
			xy p0 ( pos.x() * zoom, pos.y() * zoom ) ;
			p0 += origo;
#endif
			al_draw_tinted_scaled_rotated_bitmap_region(
				part->bitmap, part->start.x(), part->start.y(),
				part->end.x() - part->start.x(),
				part->end.y() - part->start.y(),
				color,
				part->hotspot.x(), part->hotspot.y(), p0.x(), p0.y(), zoom, zoom, pos.rad() + part->rad, 0);		
		}
		else
		{
			xy &pos1 = *pos.ref;
			xy length;
			xy height;
			xy cross;
#ifdef editor
			xy p0;
#endif
			xy p1, p2, p3;

			length.rel_x = cos(pos.rad()) * pos.length();
			length.rel_y = sin(pos.rad()) * pos.length();

			height.rel_x = cos(pos.rad() + M_PI_2) * pos.length() * 0.10f;
			height.rel_y = sin(pos.rad() + M_PI_2) * pos.length() * 0.10f;

			cross = length * 0.10f;

			p1 = pos1 + cross + height;
			p2 = pos1 + cross - height;
			p3 = pos1 + length;

#ifdef editor

			p0.set_relative_xy( pos1.x() * zoom, pos1.y() * zoom );
			p1.set_relative_xy( p1.x() * zoom, p1.y() * zoom );
			p2.set_relative_xy( p2.x() * zoom, p2.y() * zoom );
			p3.set_relative_xy( p3.x() * zoom, p3.y() * zoom );

			p0 += origo;
			p1 += origo;
			p2 += origo;
			p3 += origo;

			al_draw_line(p0.x() , p0.y() , p1.x() , p1.y() , color, 3);
			al_draw_line(p0.x() , p0.y() , p2.x() , p2.y() , color, 3);

			al_draw_line(p1.x() , p1.y() , p3.x() , p3.y() , color, 3);
			al_draw_line(p2.x() , p2.y() , p3.x() , p3.y() , color, 3);
#else

			al_draw_line(pos1.x() , pos1.y(), p1.x(), p1.y(), color, 3);
			al_draw_line(pos1.x() , pos1.y(), p2.x(), p2.y(), color, 3);

			al_draw_line(p1.x(), p1.y(), p3.x(), p3.y(), color, 3);
			al_draw_line(p2.x(), p2.y(), p3.x(), p3.y(), color, 3);
#endif

		}
	}
}

void Frame::draw(ALLEGRO_COLOR c)
{
	int n = 0;
	for (n = 0; n < boneCount; n++)
	{
		bones[n]->draw();
	}
}


int Frame::get_picked_vector(double x, double y)
{
	int n;
	int picked = -1;
	xy delta(x, y);
	xy picked_abs(1000, 1000);
	for (n = 0; n < boneCount; n++)
	{
		delta.set_relative_xy( x, y );
		delta -= bones[n]->pos;
		delta.abs();

		if (delta < picked_abs)
		{
			picked_abs = delta;
			picked = n;
		}
	}

	return picked;
}


#define bein_lengde 70

void Frame::init(double x, double y)
{
}

Frame::Frame( int BoneCount )
{
	int n;
	bones = (Bone **) malloc(sizeof(Bone *) * BoneCount);

	for (n = 0; n < BoneCount; n++)
	{
		bones[n] = new Bone();
	}

	boneCount = BoneCount;
}

Frame::~Frame()
{
	int n;

	if (bones)
	{
		for (n = 0; n < boneCount; n++)
		{
			delete bones[n];
		}

		free(bones);
		bones = NULL;
	}
}

Part *Animation::findPart(char *name)
{
	for (int p = 0; p < partCount; p++)
	{
		if (strcmp(parts[p]->name, name) == 0)
		{
			return parts[p];
		}
	}
	return NULL;
}

void Animation::__fix_bone_references(Bone **rootbones, Bone **bones)
{
	int s, find;

	for (s = 0; s < boneCount; s++)
	{
		bones[s]->color = rootbones[s]->color;
		bones[s]->pos.length_min = rootbones[s]->pos.length_min;
		bones[s]->pos.length_max = rootbones[s]->pos.length_max;
		bones[s]->part = rootbones[s]->part;

		if (rootbones[s]->connectedTo)
		{
			for (find = 0; find < boneCount; find++)
			{
				if (strcmp(rootbones[s]->connectedTo, rootbones[find]->name) == 0)
				{
					bones[s]->pos.ref = &bones[find]->pos;
					break;
				}
			}
		}
	}

}

void Animation::copyBoneProperties()
{
	int s = 0, find = 0;
	int f;
	Bone **rootbones;
	Bone **bones;

	rootbones = frames[0]->bones;

	for (f = 0; f < frameCount; f++)
	{
		__fix_bone_references(rootbones, frames[f]->bones);
	}

	__fix_bone_references(rootbones, final->bones);

}
void Animation::sortBones()
{
	int a;
	bool needs_sorting = true;
	Bone *t;
	Bone **rootbones;

	if (!frames) return;
	if (!frames[0]->bones) return;
	if (!frames[0]->bones[0]) return;
	
	rootbones = frames[0]->bones;

	while (needs_sorting)
	{
		needs_sorting = false;
		for (a = 0; a < boneCount - 1; a++)
		{
			if (rootbones[a]->sort>rootbones[a + 1]->sort)
			{
				for (int f = 0; f < frameCount; f++)
				{
					t = frames[f]->bones[a];
					frames[f]->bones[a] = frames[f]->bones[a + 1];
					frames[f]->bones[a + 1] = t;
				}

				needs_sorting = true;
			}
		}
	}
}

void Animation::transform_animation(double p, Frame &before, Frame &current, Frame &after)
{
	int n = 0;
	xy tmp;
	xy delta;
	xy part;
	xy partof;

	// calulate speed ?

	for (n = 0; n < current.boneCount; n++)
	{

#ifdef relative
		delta.set_relative_xy( after.bones[n]->pos.rel_x - current.bones[n]->pos.rel_x, after.bones[n]->pos.rel_y - current.bones[n]->pos.rel_y );

		tmp = current.bones[n]->pos.get_relative();
		partof = delta * p;
		tmp +=  partof;

		final->bones[n]->pos.rel_x = tmp.rel_x;
		final->bones[n]->pos.rel_y = tmp.rel_y;
		final->bones[n]->pos.limit();

#endif

	}
}



Animation::Animation(int FrameCount, int PartCount, int BoneCount)
{
	int n;

	frames = NULL;
	parts = NULL;

	if (FrameCount>0) frames = (Frame **) malloc(sizeof(Frame *) * FrameCount);
	if (PartCount>0) parts = (Part **)malloc(sizeof(Part  *)* PartCount);

	final = new Frame( BoneCount);

	for (n = 0; n < FrameCount; n++)
	{
		frames[n] = new Frame(BoneCount);
	}

	for (n = 0; n < partCount; n++)
	{
		parts[n] = NULL;
	}

	partCount = PartCount;
	boneCount = BoneCount;
	frameCount = FrameCount;
}

void Animation::setBitmap(ALLEGRO_BITMAP *bm)
{
	for (int n = 0; n < partCount; n++)
	{
		parts[n]->bitmap = bm;
	}
}

Animation::~Animation()
{
	int n;

	if (final) delete final;

	if (parts)
	{
		for (n = 0; n < partCount; n++)
		{
			delete parts[n];
		}

		free(parts);
		parts = NULL;
	}

	if (frames)
	{
		for (n = 0; n < frameCount; n++)
		{
			delete frames[n];
		}

		free(frames);
		frames = NULL;
	}
}


#ifndef liniar_movement
#ifdef relaive_controlls

void Frame::reinit_controll_points(Frame &before, Frame &after)
{
	int n;
	double dx, dy;
	double a;
	double cl;
	for (n = 0; n < e_bone_count; n++)
	{
		dx = after.bone[n].pos.rel_x - before.bone[n].pos.rel_x ;
		dy = after.bone[n].pos.rel_y - before.bone[n].pos.rel_y ;

		cl = sqrt((dx *dx) + (dy*dy)) * 0.25f;

		if (dx>0)
		{
			a = dy / dx;

			control_points[n * 2 + 0] = { bone[n].pos.rel_x - cl, bone[n].pos.rel_y + (a * -cl) };
			control_points[n * 2 + 1] = { bone[n].pos.rel_x + cl, bone[n].pos.rel_y + (a * cl) };
		}
		else
		{
			control_points[n * 2 + 0] = { bone[n].pos.rel_x , bone[n].pos.rel_y};
			control_points[n * 2 + 1] = { bone[n].pos.rel_x , bone[n].pos.rel_y};
		}
	}
}

#else

void Frame::reinit_controll_points(Frame &before, Frame &after)
{
	int n;
	double dx, dy;
	double a;
	for (n = 0; n < e_bone_count; n++)
	{
		dx = after.bone[n].pos.x() - before.bone[n].pos.x();
		dy = after.bone[n].pos.y() - before.bone[n].pos.y();

		if (dx>0)
		{
			a = dy / dx;

			control_points[n * 2 + 0] = { bone[n].pos.x() - 10, bone[n].pos.y() + (a * -10) };
			control_points[n * 2 + 1] = { bone[n].pos.x() + 10, bone[n].pos.y() + (a * 10) };
		}
		else
		{
			control_points[n * 2 + 0] = { bone[n].pos.x(), bone[n].pos.y() };
			control_points[n * 2 + 1] = { bone[n].pos.x(), bone[n].pos.y() };
		}
	}
}


#endif


void Frame::sort_controll_points(Frame &before, Frame &after)
{
	xy xy_temp;
	xy ghost0,ghost1;

	for (int n = 0; n < e_bone_count; n ++)
	{
		ghost0 = control_points[n * 2 + 0] - before.control_points[n * 2 + 1];
		ghost1 = after.control_points[n * 2 + 0] - control_points[n * 2 + 1];

		if (if_intersect(before.control_points[n * 2 + 1], ghost0, control_points[n * 2 + 1], ghost1))
		{
			xy_temp = control_points[n * 2 + 0];
			control_points[n * 2 + 0] = control_points[n * 2 + 1];
			control_points[n * 2 + 1] = xy_temp;
		}
	}
}

#endif
