#include "Rect.h"

#include <algorithm>

Rect::Rect(void) :
m_left(0), m_right(0), m_bottom(0), m_top(0)
{
}

Rect::Rect(const float left, const float right, 
		   const float bottom, const float top) :
m_left(left), m_right(right), m_bottom(bottom), m_top(top)
{
	validate();
}

void Rect::bottomLeft(const float x, const float y)
{
	m_left = x;
	m_bottom = y;
}


void Rect::topRight(const float x, const float y)
{
	m_right = x;
	m_top = y;
}

float Rect::width(void) const
{
	return (m_right - m_left);
}

float Rect::height(void) const
{
	return (m_top - m_bottom);
}

void Rect::validate(void)
{
	if (m_left > m_right) {
		std::swap(m_left, m_right);
	}

	if (m_bottom > m_top) {
		std::swap(m_bottom, m_top);
	}
}
