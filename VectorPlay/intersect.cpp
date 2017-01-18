
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


#ifdef amigaos4
#include "main_amigaos4.h"
#else
#include "main_windows.h"
#endif

double intersect_vector(double a0, double y0, double a1, double y1)
{
	double x;
	x = (y0 - y1) / (a1 - a0);
	return x;
}

bool if_intersect(xy pos0, xy delta0, xy pos1, xy delta1)
{
	double t;
	double x;
	double y;
	double _min, _max;

	x = intersect_vector(delta0.y() / delta0.x(), delta0.height_at_origo(pos0), delta1.y() / delta1.x(), delta1.height_at_origo(pos1));

	y = (delta0.y() / delta0.x()) * x + delta0.height_at_origo(pos0);

	//	al_draw_textf(font, al_map_rgb(255, 255, 255), x, y-20, 0, (char *) "%0.0f > %0.0f < %0.0f", pos0.x(),x , pos0.x() + delta0.x() );

	_min = pos0.x();
	_max = pos0.x() + delta0.x();

	if (_min > _max)
	{
		t = _min;
		_min = _max;
		_max = t;
	}

	if (x >= _min && x < _max) return true;

	return false;
}
