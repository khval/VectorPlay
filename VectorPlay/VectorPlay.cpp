
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
#include "simpleXML.h"


#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>

int selected_frame = 0;

bool quit = false;
bool play = false;
bool display_mode = false;

xy display_size(1920,1020);
xy mousePos;
xy origo;
xy him;

ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer = NULL;

extern char *BasicFileSave(int *errorcode);
extern char *BasicFileOpen(int *errorcode);

Animation *anim = NULL;

//Frame *animation;

ALLEGRO_FONT *font;
ALLEGRO_BITMAP *sprite_map;

double zoom = 8.0f;
double speed = 1;
int delay = 20;

int mouse_picked_corner = 0;
int picked_vector = -1;
bool MouseIsDown = false;

extern void BasicFileOpen();
//void copy_bone_data(Bone **rootbones, Bone **currentbones);
void draw_all_curvs();
void load_spritemap();
void save_xml(Animation *anim, char *filename);
Animation *load_xml(char *filename);

char *imagename = NULL;

Button quit_button(10, 10, NULL, NULL, 50, 20, "Quit");
Button load_button(0, 10, &quit_button, NULL, 50, 20, "Load");
Button save_button(0, 10, &load_button, NULL, 50, 20, "Save");
Button play_button(0, 10, &save_button, NULL, 50, 20, "Play");

Button viewmode_button(0, 10, &play_button, NULL, 80, 20, "view mode");
Button zoom_in_button(0, 10, &viewmode_button, NULL, 80, 20, "Zoom in");
Button zoom_out_button(0, 10, &zoom_in_button, NULL, 80, 20, "Zoom out");
Button speed_down_button(0, 10, &zoom_out_button, NULL, 120, 20, "Speed down");
Button speed_up_button(0, 10, &speed_down_button, NULL, 120, 20, "Speed up");
Button center_button(0, 10, &speed_up_button, NULL, 120, 20, "Center");


Button timeline_button(10, display_size.y() - 30, NULL, NULL, display_size.x()-20, 20, "");
Button workspace(10, 0, NULL, &quit_button, 1, 1, "");
Button *buttons[] = { &quit_button, &play_button, &load_button, &save_button, &timeline_button, &viewmode_button, &zoom_in_button, &zoom_out_button, &speed_down_button, &speed_up_button, &workspace, &center_button, NULL };


void draw_time_line(Button *me);
void draw_time_frames(Button *me);

int viewmode = 0;

void viewmode_button_click(int mousex, int mousey)
{
	viewmode = (viewmode + 1) % 2;
}

void quit_button_click(int mousex, int mousey)
{
	quit = true;
}

#ifdef liniar_movement
void prepare_anim()
{
}
#else
void prepare_anim()
{
	int frame;
	int frame0;
	int frame2;

	for (frame = 0; frame < frames; frame++)
	{
		frame0 = frame - 1 < 0 ? frames - 1 : frame - 1;
		frame2 = frame + 1 >= frames ? 0 : frame + 1;

		animation[frame].reinit_controll_points(animation[frame0], animation[frame2]);
	}

	for (frame = 0; frame < frames; frame++)
	{
		frame0 = frame - 1 < 0 ? frames - 1 : frame - 1;
		frame2 = frame + 1 >= frames ? 0 : frame + 1;

		animation[frame].sort_controll_points(animation[frame0], animation[frame2]);
	}
}
#endif

void play_button_click(int mousex, int mousey)
{
	play = !play;

	if (play)
	{
		him.rel_x = 0;
		him.rel_y = 0;

		timeline_button.draw_fn = draw_time_line;
		prepare_anim();
	}
	else
	{
		timeline_button.draw_fn = draw_time_frames;
	}
}


void zoom_in_button_click(int mousex, int mousey)
{
	zoom *= 2;
	
}

void zoom_out_button_click(int mousex, int mousey)
{
	zoom /= 2;

	if (zoom < (1.0f / 8.0f)) zoom = (1.0f / 8.0f);
}

void speed_down_button_click(int mousex, int mousey)
{
	speed /= 2.0f;

	if (speed < (1.0f / 8.0f)) speed = 1.0f / 8.0f;
}

void center_button_click(int mousex, int mousey)
{
	int n, b;

	for (n = 0; n < anim->frameCount; n++)
	{
		for (b = 0; b < anim->boneCount; b++)
		{
			if (anim->frames[n]->bones[b]->pos.ref == NULL)
			{
				anim->frames[n]->bones[b]->pos.rel_x = 0;
				anim->frames[n]->bones[b]->pos.rel_y = 0;
			}
		}
	}
}

void save_button_click(int mousex, int mousey)
{
	int error_code;
	char *name;

	// this is a Windows and Linux bug fix, 
	// the event que can fill up so badly the program can't handel anything else then FPS timer.
	// on AmigaOS the event que is not in a thread, so the event que won't get filled up.

	al_unregister_event_source(event_queue, al_get_timer_event_source(timer));

	if (name = BasicFileSave(&error_code))
	{
		save_xml(anim,name);
		free(name);
	}

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
}

void load_button_click(int mousex, int mousey)
{
	int error_code;
	char *name;

	al_unregister_event_source(event_queue, al_get_timer_event_source(timer));

	if (name = BasicFileOpen(&error_code))
	{
		delete anim;

		anim = load_xml(name);

		if (!anim)
		{
			anim = new Animation(0, 0, 0);
		}
		else
		{
			load_spritemap();

			anim->setBitmap(sprite_map);
			anim->sortBones();
			anim->copyBoneProperties();
			anim->limit();
		}

		free(name);
	}

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
}

void speed_up_button_click(int mousex, int mousey)
{
	speed *= 2;
}

void click_timeline(int mousex, int mousey)
{
	selected_frame = anim -> frameCount * mousex / timeline_button.width;
}

void workspace_click(int mousex, int mousey)
{
	MouseIsDown = true;

	if ((anim->frames) && (selected_frame < anim->frameCount))
	{
		picked_vector = anim->frames[selected_frame]->get_picked_vector((workspace.x() + mousex - origo.x()) / zoom, (workspace.y() + mousey - origo.y()) / zoom);
	}
}

Bone *last_low_bone = NULL;
xy last_low_bone_xy = {0,0};
xy speed_xy;
xy feet_down;
xy new_rootbone_pos;


void workspace_draw(Button *me)
{
	double x, y;
	double lx, ly;

	if (play) 
	{
		int last_selected_frame;
		int next_selected_frame;
		Bone *bone;
		Bone *rootbone;

		last_selected_frame = selected_frame - 1 < 0 ? anim->frameCount - 1 : selected_frame - 1;
		next_selected_frame = selected_frame + 1 >= anim->frameCount ? selected_frame + 1 - anim->frameCount : selected_frame + 1;

		if (selected_frame < anim->frameCount)
		{
			anim->transform_animation((double)delay / 100.0f, *anim->frames[last_selected_frame], *anim->frames[selected_frame], *anim->frames[next_selected_frame]);
		}

//		draw_all_curvs();

		y = 0;

		if (bone = anim->final->find_low_bone())
		{
			rootbone = anim->final->find_zero_bone();

			rootbone->pos = { 0, 0 };

//			al_draw_filled_rectangle( 10,10,20,20, al_map_rgba(255, 0, 0, 255));

			speed_xy.rel_x = 0;
			speed_xy.rel_y = 0;

			if (bone != last_low_bone)
			{
				him += (bone->pos - feet_down);

				// feet is down.
				feet_down = bone->pos;				
			}

//			if (bone == last_low_bone)
			{
				new_rootbone_pos = (bone->pos - feet_down) - bone -> pos;
			}


			last_low_bone = bone;
			last_low_bone_xy = bone->pos;

			rootbone->pos = new_rootbone_pos + him;
		}

//		anim->rootbone;

		x = feet_down.x() * zoom;
		y = feet_down.y() * zoom;

		anim->final->draw(al_map_rgb(255, 0, 255));
		al_draw_filled_rectangle(display_size.x() / 2 + x - 5, display_size.y() / 2 + y - 5, display_size.x() / 2 + x + 5, display_size.y() / 2 + y + 1, al_map_rgba(255, 0, 0, 255));
//		al_draw_filled_rectangle(display_size.x() / 2 + lx - 5, display_size.y() / 2 + ly - 5, display_size.x() / 2 + lx + 5, display_size.y() / 2 + ly + 1, al_map_rgba(0, 0, 255, 255));
	}
	else
	{
		int last_selected_frame;
		int next_selected_frame;

		last_selected_frame = selected_frame - 1 < 0 ? anim->frameCount - 1 : selected_frame - 1;
		next_selected_frame = selected_frame + 1 >= anim->frameCount ? selected_frame + 1 - anim->frameCount : selected_frame + 1;

		if ((anim) && (anim->frames) && (selected_frame<anim->frameCount))
		{
			al_draw_textf(font, al_map_rgba(255, 255, 255, 255), 50, 50, 0, "bones %d", anim->boneCount);

			for (int i = 0; i < anim->frames[selected_frame]->boneCount; i++)
			{
				al_draw_textf(font, al_map_rgba(255, 255, 255, 255), 50, 80 + (i * 20), 0, "%0.2lf - %0.2lf - ref %s",
					anim->frames[selected_frame]->bones[i]->pos.x(),
					anim->frames[selected_frame]->bones[i]->pos.y(),
					anim->frames[selected_frame]->bones[i]->pos.ref ? "yes" : "no"
					);
			}

			if (selected_frame < anim->frameCount)
			{
				anim->frames[selected_frame]->draw(al_map_rgb(255, 255, 255));
			}
		}
	}
}

void draw_time_frames(Button *me)
{
	int n = 0;
	double part_x;
	double _x = 0;
	double x1, y1, x2, y2;

	al_draw_rectangle(me->x(), me->y(), me->x()+me->width, me->y()+me->height, al_map_rgb(255, 255, 255),1);

	if (anim->frameCount == 0)
	{
		x1 = me -> x();
		x2 = me -> x() + me->width;
		y1 = me -> y();
		y2 = me -> y() + me->height;

		al_draw_filled_rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2, al_map_rgb(255, 255, 255));
	}
	else
	{
		part_x = me->width / anim->frameCount;
		_x = part_x + me->x();

		x1 = selected_frame * part_x + me->x();
		x2 = x1 + part_x;
		y1 = me->y();
		y2 = me->y() + me->height;

		al_draw_filled_rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2, al_map_rgb(255, 255, 255));

		for (n = 0; n < anim->frameCount - 1; n++)
		{
			al_draw_line(_x, y1, _x, y2, al_map_rgb(255, 255, 255), 1);
			_x += part_x;
		}
	}
}

void draw_time_line(Button *me)
{
	ALLEGRO_COLOR _color;
	int n = 0;
	double part_x;
	double _x = 0;
	double _hh = 0;
	double x1, y1, x2, y2;

	_color = al_map_rgba(255, 255, 255, 255);

	if (anim->frameCount == 0)
	{
		x1 = me->x();
		x2 = me->x() + me->width;
		y1 = me->y();
		y2 = me->y() + me->height;

		al_draw_filled_rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2, _color );
	}
	else
	{
		part_x = me->width / anim->frameCount;
		

		x1 = selected_frame * part_x;
		x2 = x1 + part_x;
		y1 = me->y();
		y2 = me->y() + me->height;

		_hh = y1 + (me->height / 2);

		al_draw_line(
			me->x(), _hh, 	
			me->x()+me->width, _hh, _color, 1);

		al_draw_circle(
			me->x() + x1 + (part_x * delay / 100.0f), _hh - 1, 4, _color, 1);

		_x = me->x();
		for (n = 0; n < anim->frameCount +1; n++)
		{
			al_draw_line(_x, y1, _x, y2, _color, 1);

			al_draw_line(_x + (part_x / 2) , _hh - 3, _x + (part_x / 2), _hh + 3, _color, 1);

			_x += part_x;
		}
	}
}


void draw_play(Button *me)
{
	double sx, sy;
	if (play)
	{
		al_draw_filled_rectangle(me->x()+1, me->y()+1, me->x() + me->width -1 , me->y() + me->height -1, al_map_rgb(0, 0, 255));
	}
	if (me->text)
	{
		sx = me->width / 2 - (al_get_text_width(font, me->text) / 2);
		sy = me->height / 2 - (al_get_font_line_height(font) / 2);

		al_draw_textf(font, al_map_rgb(255, 255, 255), me->x() + sx, me->y() + sy, 0, (char *) "%s", me->text);
	}

}


double rot = 0;


void drawspot(xy &pos)
{
	al_draw_filled_rectangle(pos.x() - 1, pos.y() - 1, pos.x() + 1, pos.y() + 1, al_map_rgba(255, 255, 255, 255));
}


void drawsline(xy &pos0, xy &pos1)
{
	al_draw_line(pos0.x() , pos0.y() , pos1.x() , pos1.y() , al_map_rgba(255, 255, 255, 255),3);
}


void draw_angel(xy &pos0)
{
	double x, y;
	
	if (pos0.ref)
	{
		xy &pos1 = *pos0.ref;

		x = cos(pos0.rad()) * pos0.length();
		y = sin(pos0.rad()) * pos0.length();
		pos1 = *pos0.ref;
		al_draw_line(pos1.x(), pos1.y(), pos1.x() + x, pos1.y() + y, al_map_rgba(255, 255, 255, 255), 3);
	}
}

xy curv_xy(double p,xy p0, xy p1, xy p2)
{
	xy d0;
	xy d1;
	xy d;
	
	// find delta
	d0 = ((p1 - p0) * p);
	// add start
	d0 += p0;
	
	// find delta
	d1 = ((p2 - p1) * p);
	// add start
	d1 += p1;

	// find delta
	d = (d1 - d0)*p;
	// add start
	d += d0;

	return d;
}
#ifdef relaive_controlls
void curv( xy *ref, xy p0, xy p1, xy p2)
#else
void curv(xy p0, xy p1, xy p2)
#endif
{
	xy d0;
	xy d1;
	xy d;
	double p;

	for (p = 0.0f; p < 1.0f; p += 0.1f)
	{
		// find delta
		d0 = ((p1 - p0) * p);
		// add start
		d0 += p0;

		// find delta
		d1 = ((p2 - p1) * p);
		// add start
		d1 += p1;

		// find delta
		d = (d1 - d0)*p;
		// add start
		d += d0;

#ifdef relaive_controlls

		if (ref)
		{
			al_draw_pixel(ref-> x() + d.rel_x, ref->y() + d.rel_y, al_map_rgba(255, 255, 255, 255));
		}
		else
		{
			al_draw_pixel(d.rel_x, d.rel_y, al_map_rgba(255, 255, 255, 255));
		}
#else
		al_draw_pixel(d.x(), d.y(), al_map_rgba(255, 255, 255, 255));
#endif

	}
}

void draw_all_curvs()
{
	int last_selected_frame;
	int next_selected_frame;
	int frame;
	double p;

	for (frame = 0; frame < anim->frameCount; frame++)
	{
		last_selected_frame = frame - 1 < 0 ? anim->frameCount - 1 : frame - 1;
		next_selected_frame = frame + 1 >= anim->frameCount ? frame + 1 - anim->frameCount : frame + 1;

		for (p = 0; p < 1.0f; p += 0.1f)
		{
//			transform_animation_visual(p, *anim->frames[last_selected_frame], *anim->frames[frame], *anim->frames[next_selected_frame]);
		}
//		DPrintF("%d - %d - %d\n", last_selected_frame, frame, next_selected_frame);
	}
}


void printV(xy &pos)
{
//	DPrintF("%lf,%lf\n", pos.x(), pos.y());
}




void resize_buttons()
{
	int n = 0;

	if (font)
	{
		int bx, by;
		int w, h, hh;

		hh = 0;

		for (Button **button = buttons; *button != NULL; button++)
		{
			(*button)->number = n++;


			if ((*button)->text)
			{
				if (strlen((*button)->text) > 0)
				{
					al_get_text_dimensions(font, (*button)->text, &bx, &by, &w, &h);
					(*button)->width = w + 20;
					if (h > hh) hh = h;
				}
			}
		}

		for (Button **button = buttons; *button != NULL; button++)
		{
			if ((*button)->text)
			{
				if (strlen((*button)->text) > 0)
				{
					(*button)->height = hh + 10;
				}
			}
		}
	}
}

Animation *load_xml( char *filename )
{
	SimpleXML doc;
	SimpleXML *EditorObj;
	SimpleXML *framesObj;
	SimpleXML *frame;
	SimpleXML *bonesObj;
	SimpleXML *bone;
	SimpleXML *partsObj;
	SimpleXML *part;
	Frame *rootframe;
	char *tmpname;

	unsigned int argb;

	Animation *anim = NULL;

	int parts_count = 0;
	int bones_count = 0;
	int frames_count = 0;

	doc.load( filename );

	if (EditorObj = doc.get_object("editor", 1))
	{
		char *temp_name;

		zoom = EditorObj->get_double_value("zoom");
		temp_name = EditorObj->get_str_value("image");
		delay = EditorObj->get_int_value("speed");

		if (temp_name)
		{
			if (imagename) free(imagename);
			imagename = temp_name;
		}

		delete EditorObj;
	}

	if (partsObj = doc.get_object("parts", 1))
	{
		parts_count = partsObj->count("part");
		delete partsObj;
	}

	if (bonesObj = doc.get_object("bones", 1))
	{
		bones_count = bonesObj->count("bone");
		delete bonesObj;
	}

	if (framesObj = doc.get_object("frames", 1))
	{
		frames_count = framesObj->count("frame");
		delete framesObj;
	}

	anim = new Animation(frames_count, parts_count, bones_count);

	if (!anim) return NULL;

	// Get all info about the skeleton.

	if (partsObj = doc.get_object("parts", 1))
	{
		char *name = NULL;
		xy start;
		xy end;
		xy hotspot;
		double rad;

		parts_count = 0;
		while (part = partsObj->get_object("part", parts_count + 1))
		{
			name = part->get_str_value("name");
			hotspot = part->get_xy_value("hotspot");
			start = part->get_xy_value("start");
			end = part->get_xy_value("end");
			rad = part->get_double_value("degrees") * 2 * M_PI / 360.0f;

			anim->parts[parts_count] = new Part(name, NULL, start, end, hotspot, rad);

			if (name) free(name);
			name = NULL;

			parts_count++;
			delete part;
		}

		delete partsObj;
	}

	if (bonesObj = doc.get_object("bones", 1))
	{
		Bone *theBone;

		rootframe = (bones_count > 0) ? anim->frames[0] : NULL;

		bones_count = 0;
		while (bone = bonesObj->get_object("bone", bones_count + 1))
		{
			theBone = rootframe->bones[bones_count];

			argb = bone->get_hex_value("color");
			theBone->color = al_map_rgba((argb & 0xFF0000) >> 16, (argb & 0xFF00) >> 8, argb & 0xFF, (argb & 0xFF000000) >> 24);
			theBone->name = bone->get_str_value("name");
			theBone->connectedTo = bone->get_str_value("connectedTo");
			theBone->pos.length_min = bone->get_double_value("min");
			theBone->pos.length_max = bone->get_double_value("max");
			theBone->sort = bone->get_int_value("sort");

			if (char *partname = bone->get_str_value("part"))
			{
				theBone->part = anim->findPart(partname);
				free(partname);
			}

			bones_count++;
			delete bone;
		}

		delete bonesObj;
	}


	if (framesObj = doc.get_object("frames", 1))
	{
		frames_count = 0;

		while (frame = framesObj->get_object("frame", frames_count + 1))
		{
			bones_count = 0;

			if (bonesObj = frame->get_object("bones", 1))
			{
				while (bone = bonesObj->get_object("bone", bones_count + 1))
				{
					anim->frames[frames_count]->bones[bones_count]->pos.rel_x = bone->get_double_value("x");
					anim->frames[frames_count]->bones[bones_count]->pos.rel_y = bone->get_double_value("y");

					bones_count++;
					delete bone;
				}
				delete bonesObj;
			}
			frames_count++;
			delete frame;
		}

		delete framesObj;
	}

	if (zoom < (1.0f / 8.0f)) zoom = (1.0f / 8.0f);

	return anim;
}

void save_xml(Animation *anim, char *filename)
{
	FILE *fd;
	int a;

	if (!anim) return;

	if (fd = fopen(filename, "w"))
	{
		fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf(fd, "<main>\n");

		fprintf(fd, "<editor>\n");
		fprintf(fd, "<zoom>%0.2lf</zoom>\n", zoom);
		fprintf(fd, "<image>%s</image>\n", imagename ? imagename : "none");
		fprintf(fd, "<speed>%d</speed>\n", delay);
		fprintf(fd, "</editor>\n");

		fprintf(fd, "<parts>\n");
		for (int p = 0; p < anim->partCount; p++)
		{
			if (anim->parts[p])	anim->parts[p]->save(fd);
		}
		fprintf(fd, "</parts>\n");

		fprintf(fd, "<bones>\n");
		for (int b = 0; b < anim->boneCount; b++)
		{
			anim->frames[0]->bones[b]->save(fd);
		}
		fprintf(fd, "</bones>\n");

		fprintf(fd, "<frames>\n");
		for (a = 0; a < anim->frameCount; a++)
		{
			fprintf(fd, "<frame>\n");

			fprintf(fd, "<bones>\n");
			for (int n = 0; n < anim->boneCount; n++)
			{
				anim->frames[a]->bones[n]->save_pos(fd);
			}
			fprintf(fd, "</bones>\n");

			fprintf(fd, "</frame>\n");
		}
		fprintf(fd, "</frames>\n");
		fprintf(fd, "</main>\n");

		fclose(fd);
	}

}

void keyboard(int keycode)
{
	int frame = selected_frame;
	int last_selected_frame, next_selected_frame;
	int bone;
	xy *ref;
	

	if (keycode == ALLEGRO_KEY_C)
	{
		last_selected_frame = frame - 1 < 0 ? anim->frameCount - 1 : frame - 1;
		next_selected_frame = frame + 1 >= anim->frameCount ? frame + 1 - anim->frameCount : frame + 1;

		for (bone = 0; bone < anim -> boneCount; bone++)
		{
			ref = anim -> frames[frame]->bones[bone]->pos.ref;
			anim->frames[frame]->bones[bone]->pos = anim->frames[last_selected_frame]->bones[bone]->pos;
			anim->frames[frame]->bones[bone]->pos.ref = ref;
		}

	}
}

void load_spritemap()
{
	if (imagename)
	{
		ALLEGRO_BITMAP *tmp;

		tmp = al_load_bitmap(imagename);

		if (tmp)
		{
			if (sprite_map) al_destroy_bitmap(sprite_map);
			sprite_map = tmp;
		}
	}
}

int main(int argc, char* argv[])
{
	ALLEGRO_DISPLAY *disp;

	ALLEGRO_EVENT event;
	ALLEGRO_BITMAP *display_bitmap = NULL;
	ALLEGRO_MOUSE_STATE mstate;

	char buffer[1000];



#ifdef __amigaos4__
	amiga_init();
#endif

	if (!al_init())	return -1;

	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
	disp = al_create_display(display_size.x(), display_size.y());
	event_queue = al_create_event_queue();

	timer = al_create_timer(1.0f/60.0f);

	if (disp)
	{
		display_bitmap = al_get_target_bitmap();

		al_init_font_addon();
		al_init_ttf_addon();
		al_init_primitives_addon();
		al_init_font_addon();
		al_init_image_addon();
		al_install_mouse();
		al_install_keyboard();

#ifndef amigaos4
		font = al_load_font("fonts/orbitron black.ttf", 15, 0);
#else
		font = al_load_font("Century Schoolbook L Bold.font", 15, 0);
#endif

	}

	origo = display_size / 2;

	anim = load_xml("anim.xml");

	if (!anim)
	{
		anim = new Animation(0, 0, 0);
		load_spritemap();
	}
	else
	{
		load_spritemap();

		anim->setBitmap(sprite_map);
		anim->sortBones();
		anim->copyBoneProperties();

	}

	resize_buttons();
	
	load_button.click_fn = load_button_click;
	save_button.click_fn = save_button_click;
	
	quit_button.click_fn = quit_button_click;
	timeline_button.click_fn = click_timeline;
	timeline_button.draw_outline_enable = false;
	
	timeline_button.draw_fn = draw_time_frames;
	play_button.draw_fn = draw_play;


	viewmode_button.click_fn = viewmode_button_click;

	zoom_in_button.click_fn = zoom_in_button_click;
	zoom_out_button.click_fn = zoom_out_button_click;
	play_button.click_fn = play_button_click;
	speed_down_button.click_fn = speed_down_button_click;
	speed_up_button.click_fn = speed_up_button_click;

	center_button.click_fn = center_button_click;

	workspace.click_fn = workspace_click;
	workspace.draw_fn = workspace_draw;
	workspace.width = display_size.x() - (workspace.x() * 2);
	workspace.height =  timeline_button.y() - 10 - workspace.y() ;

	getcwd(buffer, sizeof(buffer));

	if (!font) printf("Font not found\n");
	if (!disp) printf("display is 0x%X08\n", disp);
	if (!display_bitmap) printf("fount not found %s\n", display_bitmap);

	if ((event_queue) && (disp) && (font))
	{
		int n0 = 0, n1 = 0;

		double endX = 0;
		double startX = 0;
		double workspace_selected = false;
		double slider_selected = false;

		al_register_event_source(event_queue, al_get_display_event_source(disp));
		al_register_event_source(event_queue, al_get_keyboard_event_source());
		al_register_event_source(event_queue, al_get_mouse_event_source());
		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		al_start_timer(timer);

		while (!quit)
		{
			al_get_mouse_state(&mstate);
			al_wait_for_event(event_queue, &event);

			if (event.type == ALLEGRO_EVENT_TIMER)
			{
//				al_draw_textf(font, al_map_rgba(255, 255, 255, 255), 350, 50, 0, "%0.2lf - %0.2lf - %0.2lf", origo.x(), origo.y(), zoom);

				al_draw_textf(font, al_map_rgb(255, 255, 255), 800, 50, 0, "%i, %i  ", mstate.x, mstate.y, mstate.buttons );
					

				if (font)
				{
					for (Button **button = buttons; *button != NULL; button++)
					{
						if ((*button)->text)
						{
							(*button)->Draw();
						}
					}
				}

				al_flip_display();
				al_clear_to_color(al_map_rgba(100, 100, 100, 255));
			}
	
			if (event.type != ALLEGRO_EVENT_TIMER)
			{
				for (Button **button = buttons; *button != NULL; button++)
				{
					if ((*button)->Event(event.type, mstate.x, mstate.y))
					{
						
					}
				}
			}
			else
			{
				if (play)
				{
					delay = delay + speed;

					if ((delay < 0) || ( delay >= 100))
					{
						if (delay < 0) delay += 100;

						delay = delay % 100;

						selected_frame = (selected_frame + 1) % anim->frameCount;
					}
				}
			}

			switch (event.type)
			{
				case ALLEGRO_EVENT_KEY_DOWN:
					keyboard(event.keyboard.keycode);
					break;

				case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
					break;

				case ALLEGRO_EVENT_MOUSE_BUTTON_UP:

					prepare_anim();
					MouseIsDown = false;
					break;
			}


			if ((anim) && (anim ->frames) && (selected_frame<anim->frameCount))
			{
				if ((mstate.y > 20) && (mstate.y < display_size.y() - 20))
				{
					if (MouseIsDown && picked_vector >-1)
					{
						anim->frames[selected_frame]->bones[picked_vector]->pos.set_absolute_xy( (mstate.x - origo.x()) / zoom, (mstate.y - origo.y()) / zoom);
					}
				}
			}
		}
	}

	if (anim)
	{
		save_xml(anim,"anim.xml");
		delete anim;
		anim = NULL;
	}

	if (sprite_map) al_destroy_bitmap(sprite_map);
	if (timer) al_destroy_timer(timer);
	if (font) al_destroy_font(font);
	if (event_queue) al_destroy_event_queue(event_queue);
	if (disp) al_destroy_display(disp);

#ifdef __amigaos4__
		amiga_uninit();
#endif

	return 0;
}

