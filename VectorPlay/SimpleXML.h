
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


#pragma once
#include <String>

#ifdef amigaos4
#include "main_amigaos4.h"
#endif

#ifdef windows
#include "main_windows.h"
#endif

class SimpleXML
{
	FILE *fd;
	char *buffer;

public:

	void load(char *filename);
//	void save(char *filename);
	SimpleXML *get_object(char *name, int number);

	unsigned int count(char *name);

	char *value();
	void set_buffer(char *str);
	double get_double_value(char *name);
	int get_int_value(char *name);
	unsigned int get_hex_value(char *name);
	char *get_str_value(char *name);

	xy get_xy_value(char *name);

	void operator=(SimpleXML &value);

	SimpleXML();
	~SimpleXML();
};

