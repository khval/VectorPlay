
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

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SimpleXML.h"
#include "main.h"
#include "debug.h"

#ifdef windows

char *strndup(char *str, int chars)
{
	char *buffer;
	int n;

	buffer = (char *) malloc(chars +1);
	if (buffer)
	{
		for (n = 0; ((n < chars)&&(str[n] != 0)); n++) buffer[n] = str[n];
		buffer[n] = 0;
	}

	return buffer;
}

#endif


SimpleXML::SimpleXML()
{
	buffer = NULL;
}

SimpleXML::~SimpleXML()
{
	if (buffer) free(buffer);
}

void SimpleXML::load(char *filename)
{
	unsigned long long int size;
	fd = fopen(filename, "r");

	if (fd)
	{
		_fseeki64(fd, 0, SEEK_END);
		size = ftell(fd);
		_fseeki64(fd, 0,SEEK_SET);

		if (size > 0)
		{
			buffer = (char *)malloc(size + 1);
			if (buffer)
			{
				memset(buffer, 0, size);
				fread(buffer, 1, size, fd);
			}
		}
		fclose(fd);
	}
}

SimpleXML *SimpleXML::get_object(char *name, int number)
{
	char *from = buffer;
	char *ret;
	char *end_str;
	SimpleXML *_new = NULL;
	char start_tag[100];
	char end_tag[100];
	char *_new_buffer;

	if (!buffer) return NULL;

	sprintf(start_tag, "<%s>", name);
	sprintf(end_tag, "</%s>", name);
	
	ret = buffer;

	while ((number > 0) && (ret = strstr(ret, start_tag)))
	{
		ret += strlen(start_tag);
		number--;
	}

	if (ret)
	{
		end_str = strstr(ret, end_tag);
		if (end_str)
		{
			_new_buffer = strndup(ret, end_str - ret);
			if (_new_buffer)
			{
				_new = new SimpleXML();
				if (_new) _new->set_buffer(_new_buffer);					
				free(_new_buffer);
			}
		}
	}
	return _new;
}

void SimpleXML::set_buffer(char *new_buffer)
{
	if (buffer)	free(buffer);
	buffer = strdup(new_buffer);
}

char *SimpleXML::value()
{
	return buffer;
}

void SimpleXML::operator = (SimpleXML &other)
{
	if (buffer) free(buffer);
	buffer = NULL;

	if (other.value())
	{
		buffer = strdup(other.value());
	}
}

double SimpleXML::get_double_value(char *name)
{
	double ret = 0.0f;
	SimpleXML *tmp;

	if (tmp = get_object(name, 1))
	{
		sscanf(tmp->value(), "%lf", &ret);
		delete tmp;
	}

	return ret;
}

unsigned int SimpleXML::get_hex_value(char *name)
{
	unsigned int ret = 0;
	SimpleXML *tmp;

	if (tmp = get_object(name, 1))
	{
		sscanf(tmp->value(), "%x", &ret);
		delete tmp;
	}

	return ret;
}

unsigned int SimpleXML::count(char *name)
{
	int count = 0;
	SimpleXML *obj;

	while (obj = get_object( name, count + 1))
	{
		count++;
		delete obj;
	}

	return count;
}

char *SimpleXML::get_str_value(char *name)
{
	char *ret = NULL;
	char *t;
	SimpleXML *tmp;

	if (tmp = get_object(name, 1))
	{
		t = tmp->value();
		if (t) ret = strdup(t);
		delete tmp;
	}

	return ret;
}


xy SimpleXML::get_xy_value(char *name)
{
	xy ret;
	SimpleXML *tmp;

	if (tmp = get_object(name, 1))
	{
		ret.rel_x = tmp->get_double_value("x");
		ret.rel_y = tmp->get_double_value("y");
		delete tmp;
	}
	return ret;
}