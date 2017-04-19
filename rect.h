#ifndef RECT_H_INCLUDED
#define RECT_H_INCLUDED

class Rect
{
public:
	Rect(void);
	Rect(const float left, const float right, const float bottom, const float top);

	void bottomLeft(const float x, const float y);
	void topRight(const float x, const float y);

	void validate(void);

	float left(void) const { return m_left; }
	float right(void) const { return m_right; }
	float bottom(void) const { return m_bottom; }
	float top(void) const { return m_top; }
	float width(void) const;
	float height(void) const;

	void left(float newLeft) { m_left = newLeft; }
	void right(float newRight) { m_right = newRight; }
	void bottom(float newBottom) { m_bottom = newBottom; }
	void top(float newTop) { m_top = newTop; }

protected:

	float m_left;
	float m_right;
	// note that top has greater y value
	float m_bottom;
	float m_top;
};

#endif // RECTANGLE_H_INCLUDED
