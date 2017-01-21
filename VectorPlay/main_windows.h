
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


#include "allegro5/allegro.h"
#include "allegro5/allegro_font.h"

#define windows
#define controlls_off
#define relative
#define liniar_movement
#define editor

#ifndef main_h
#define main_h
/*
enum
{
	e_hode,
	e_nake,
	e_skuldere,
	e_over_arm_hoyre,
	e_over_arm_venstre,
	e_under_arm_hoyre,
	e_under_arm_venstre,
	e_skrit,
	e_krop,
	e_arm_krop,
	e_bein_hoyre,
	e_bein_venstre,
	e_leg_hoyre,
	e_leg_venstre,
	e_fot_hoyre,
	e_fot_venstre,
	e_hond_hoyre,
	e_hond_venstre,
	e_bone_count
};
*/

class xy
{
private:

public:
	double rel_x;
	double rel_y;

	double length_min;
	double length_max;
	
	double rad_min;
	double rad_max;

	xy *ref;
	xy();
	xy(double x, double y);

	double x();
	double y();
	double length();
	double rad();

	xy operator-();
	xy operator+(xy &pos);
	xy operator-(xy &pos);
	xy operator/(xy &pos);
	xy operator*(xy &pos);
	xy operator/(double length);
	xy operator*(double length);
	xy operator+=(xy &pos);
	xy operator-=(xy &pos);
	bool operator<(xy &pos);
	bool operator>(xy &pos);

	void abs();
	void rot(double angel);
	void set_relative_xy(double x, double y);
	void set_absolute_xy(double x, double y);

#ifdef relative
	xy get_relative();
#endif

	void set_angel_vector(double rad, double length);
	double height_at_origo(xy offset);
	void limit();
};

class rect
{
	public:
	class xy p0;
	class xy p1;
	class xy p2;
	class xy p3;

	rect();
};

#endif
