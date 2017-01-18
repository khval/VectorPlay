
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

#define _USE_MATH_DEFINES
#include <math.h>

xy::xy(void)
{
	ref = NULL;
	rel_x = 0;
	rel_y = 0;
	length_max = -1;
	length_min = -1;
	rad_max = M_PI *2;
	rad_min = 0;
}


xy::xy(double x, double y)
{
	ref = NULL;
	rel_x = x;
	rel_y = y;
	length_max = -1;
	length_min = -1;
	rad_max = M_PI *2;
	rad_min = 0;
}

xy xy::get_relative()
{
	xy ret(rel_x, rel_y);
	return ret;
}

void xy::set_absolute_xy(double x, double y)
{
	rel_x = ref ? x - ref->x() : x;
	rel_y = ref ? y - ref->y() : y;

	limit();
}

void xy::set_relative_xy(double x, double y)
{
	rel_x = x;
	rel_y = y;
}

void xy::set_angel_vector(double _rad, double _length)
{
	rel_x = cos(_rad) * _length;
	rel_y = sin(_rad) * _length;
}

double xy::x()
{
	return ref ? ref->x() + rel_x : rel_x;
}

double xy::y()
{
	return ref ? ref->y() + rel_y : rel_y;
}

double xy::length()
{
	return sqrt((rel_x*rel_x) + (rel_y*rel_y));
}

double xy::rad()
{
	double l = length();
	double _r;

	_r = l > 0 ? asin(rel_y / l) : 0;

	if (rel_x < 0) _r = M_PI - _r;
	if (_r < 0) _r = 2 * M_PI + _r ;

	return _r;
}

xy xy::operator+(xy &pos)
{
	xy ret ( x() + pos.x(), y() + pos.y() );
	return ret;
}

xy xy::operator+=(xy &pos)
{
	rel_x += pos.x();
	rel_y += pos.y();
	return *this;
}


xy xy::operator-()
{
	xy ret( -x() , -y() );
	return ret;
}


xy xy::operator-(xy &pos)
{
	xy ret ( x() - pos.x(), y() - pos.y() );
	return ret;
}

xy xy::operator-=(xy &pos)
{
	rel_x -= pos.x();
	rel_y -= pos.y();
	return *this;
}


xy xy::operator/(xy &pos)
{
	xy ret ( rel_x / pos.x(), rel_y / pos.y() );
	return ret;
}

xy xy::operator*(xy &pos)
{
	xy ret ( rel_x * pos.x(), rel_y * pos.y() );
	return ret;
}

xy xy::operator/(double length)
{
	xy ret ( rel_x / length, rel_y / length );
	return ret;

}

xy xy::operator*(double length)
{
	xy ret ( rel_x * length, rel_y * length );
	return ret;
}

void xy::rot(double rot)
{
	double vx0x,vy1x;
	double vx0y,vy1y;

	vx0x = sin(rot) * rel_x;
	vx0y = cos(rot) * rel_x;

	vy1x = sin(rot + M_PI_2) * rel_y;
	vy1y = cos(rot + M_PI_2) * rel_y;

	rel_x = vx0x + vy1x;
	rel_y = vx0y + vy1y;
}

double xy::height_at_origo(xy offset)
{
	double a;

	if (rel_x != 0)
	{
		a = rel_y / rel_x;
		return offset.y() + (a * -offset.x());
	}
	return 0;
}

void xy::abs()
{
	if (rel_x < 0) rel_x = - rel_x;
	if (rel_y < 0) rel_y = - rel_y;
}

bool xy::operator<(xy &pos)
{
	return length() < pos.length();
}			

bool xy::operator>(xy &pos)
{
	return length() > pos.length();
}

void xy::limit()
{
	double _rad = rad();
	double _length = length();
	bool not_valid = true;
	
	if (length_min>0)
		_length = _length < length_min ? length_min : _length;

	if (length_max>0)
		_length = _length > length_max ? length_max : _length;

	if (rad_min > rad_max)
	{
		if ((_rad >= rad_min && _rad <= (M_PI * 2))
			||
			(_rad >= 0 && _rad < rad_max))
		{
			not_valid = false;
		}
	}
	else
	{
		not_valid = !(_rad > rad_min && _rad < rad_max);
	}
	
	if (not_valid)
	{
		// find closes limit.
		double dmax, dmin;
		dmax = rad_min - _rad;
		dmin = rad_max - _rad;
		dmin = dmin < 0 ? -dmin : dmin;
		dmax = dmax < 0 ? -dmax : dmax;
		_rad = dmin < dmax ? dmin : dmax;
	}

	rel_x = cos(_rad) * _length;
	rel_y = sin(_rad) * _length;
}

rect::rect()
{
	p0.set_relative_xy( -5, -5 );
	p1.set_relative_xy( 5, -5 );
	p2.set_relative_xy( 5, 5 );
	p3.set_relative_xy( -5, 5 );
}
