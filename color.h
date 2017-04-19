#ifndef INCLUDED_COLOR_H
#define INCLUDED_COLOR_H

class Color
{
public:
	Color(void);
	Color(const float new_red, const float new_green, const float new_blue);

	float red;
	float green;
	float blue;
};

#endif