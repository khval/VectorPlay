#include <stdio.h>
#include <stdlib.h>
#include "allegro5/allegro.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "buttons.h"

#ifdef amigaos4
#include "unistd.h"
#include "main_amigaos4.h"
#else
#include "main_windows.h"
#endif

#include "debug.h"
#include "animation.h"

#ifdef liniar_movement

void transform_animation_bone(int n, double p, xy &before, xy &current, xy &after, xy &d)
{
	xy partof;
#ifdef relative
	xy delta(after.rel_x - current.rel_x, after.rel_y - current.rel_y);
	partof = delta * p;
	d = current.get_relative() + partof;
#endif

}


#ifdef relativ

void transform_animation_bone_recursive(int n, double p, xy &before, xy &current, xy &after, xy &d)
{
	xy rec;
	xy bone;

	if (current.ref)
	{
		transform_animation_bone_recursive(n, p, *before.ref, *current.ref, *after.ref, rec);
	}

	transform_animation_bone(n, p, before, current, after, bone);

	d = bone + rec;
}


#endif

void transform_animation_visual(double p, Frame &before, Frame &current, Frame &after)
{
	int n = 0;
	xy bone;

	for (n = 0; n < current.boneCount; n++)
	{
		transform_animation_bone_recursive(n, p, before.bones[n]->pos, current.bones[n]->pos, after.bones[n]->pos, bone);

		al_draw_pixel(bone.x(), bone.y(), al_map_rgba(255, 255, 255, 255));
	}
}

#endif

#ifndef liniar_movement

void transform_animation(double p, Frame &before, Frame &current, Frame &after, Frame &d)
{
	int n = 0;
	double x, y;
	xy ret;
	xy ghost0;
	xy ghost1;

	for (n = 0; n < e_bone_count; n++)
	{
		ghost0 = current.control_points[n * 2 + 0] - before.control_points[n * 2 + 1];
		ghost1 = after.control_points[n * 2 + 0] - current.control_points[n * 2 + 1];

		if (p < 0.5f)
		{
			ret = curv_xy(p * 2,
				ghost0 / 2 + before.control_points[n * 2 + 1],
				current.control_points[n * 2 + 0],
#ifdef relaive
				{ current.bone[n].pos.rel_x, current.bone[n].pos.rel_y }
#else
				current.bone[n].pos
#endif
				);

		}
		else
		{
			ret = curv_xy((p - 0.5f) * 2,
#ifdef relaive
			{ current.bone[n].pos.rel_x, current.bone[n].pos.rel_y },
#else
				current.bone[n].pos,
#endif
				current.control_points[n * 2 + 1],
				ghost1 / 2 + current.control_points[n * 2 + 1]
				);
		}

#ifdef relaive

		d.bone[n].pos.rel_x = ret.x();
		d.bone[n].pos.rel_y = ret.y();

#else

		d.bone[n].pos.set_relative(ret.x(), ret.y());
#endif
	}

	for (n = 0; n < e_bone_count; n++)
	{
		d.bone[n].pos.limit();
	}
}


void transform_animation_visual(Frame &before, Frame &current, Frame &after)
{
	int n = 0;
	double x, y;
	xy ret;
	xy ghost0;
	xy ghost1;

	for (n = 0; n < e_bone_count; n++)
	{
		ghost0 = current.control_points[n * 2 + 0] - before.control_points[n * 2 + 1];
		ghost1 = after.control_points[n * 2 + 0] - current.control_points[n * 2 + 1];

		curv(
#ifdef relaive
			current.bone[n].pos.ref,
#endif
			ghost0 / 2 + before.control_points[n * 2 + 1],
			current.control_points[n * 2 + 0],
#ifdef relaive
			{ current.bone[n].pos.rel_x, current.bone[n].pos.rel_y }
#else
			current.bone[n].pos
#endif
			);

		curv(
#ifdef relaive
			current.bone[n].pos.ref,
#endif

#ifdef relaive
			{ current.bone[n].pos.rel_x, current.bone[n].pos.rel_y },
#else
			current.bone[n].pos,
#endif
			current.control_points[n * 2 + 1],
			ghost1 / 2 + current.control_points[n * 2 + 1]
			);

	}
}

#endif
