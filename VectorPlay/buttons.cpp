
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


#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include "buttons.h"
#include "string.h"
#include "debug.h"

using namespace std;

extern ALLEGRO_FONT *font;

Button::Button( int _x, int _y, Button *_rel_x, Button *_rel_y, int _width, int _height, char *name )
{
	text = NULL;
	__x = _x;
	__y = _y;
	rel_x = _rel_x;
	rel_y = _rel_y;
	width = _width;
	height = _height;

	click_fn = NULL;
	draw_fn = NULL;
	draw_outline_enable = true;

	if (name) 	text = strdup(name);
}

int Button::x()
{
	int x = __x;
	if (rel_x) x = rel_x -> x() + rel_x -> width + 5;
	return x;
}

int Button::y()
{
	int y = __y;
	if (rel_y) y = rel_y -> y() + rel_y -> height + 5;
	return y;
}

void Button::Draw()
{
	int sx, sy;

	if (draw_outline_enable) al_draw_rectangle(x() + 1, y() + 1, x() + width, y() + height, al_map_rgb(255, 255, 255), 1);

	if (draw_fn)
	{
		draw_fn(this);
	}
	else
	{
		if (text)
		{
			sx = width / 2 - (al_get_text_width(font, text) / 2);
			sy = height / 2 - (al_get_font_line_height(font) / 2);

			al_draw_textf(font, al_map_rgb(255, 255, 255), x() + sx, y() + sy, 0, (char *) "%s", text);
		}
	}
}

bool Button::Event( int event, int mousex,int mousey )
{
	int _x, _y;
	
	_x = x();
	_y = y();

	if ((mousex > _x ) && (mousex < _x + width )&& (mousey > _y) && (mousey < _y + height))
	{

		DPrintF("debug %d   %d,%d,%d --- %d\n", event, number, mousex, mousey, _y + height);

		switch (event)
		{
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
					if (click_fn) click_fn( mousex - x(), mousey - y() );
					return true;
					break;			
		}
	}
	return false;
}

Button::~Button()
{
	if (text) free (text);
	text = NULL;
}