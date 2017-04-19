#ifndef CURVE_H_INCLUDED
#define CURVE_H_INCLUDED

#pragma warning(disable : 4786)

#include <vector>
#include <iostream>
#include <string>

#include "Point.h"

class CurveEvaluator;

//using namespace std;

class Curve
{
public:

	Curve();
	Curve(const float fMaxX, const float fStartYValue);
	Curve(const float fMaxX, const Point& point);
	Curve(std::istream& isInputStream);

	void maxX(const float fNewMaxX);
	void setEvaluator(const CurveEvaluator* pceEvaluator) { m_pceEvaluator = pceEvaluator; }
	float evaluateCurveAt(const float x) const;
	void scaleX(const float fScale);
	void addControlPoint(const Point& point);
	void removeControlPoint(const int iCtrlPt);
	void removeControlPoint2(const int iCtrlPt);
	void getControlPoint(const int iCtrlPt, Point& ptCtrlPt) const {
		ptCtrlPt = m_ptvCtrlPts[iCtrlPt];
	}
	int getClosestControlPoint(const Point& point, Point& ptCtrlPt) const;
	void getClosestPoint(const Point& pt, Point& ptClosestPt) const;
	float getDistanceToCurve(const Point& normalized_point) const;
	void moveControlPoint(const int iCtrlPt, const Point& ptNewPt);
	void moveControlPoints(const std::vector<int>& ivCtrlPts, const Point& ptOffset,
		const float fMinY, const float fMaxY);

	int controlPointCount(void) const;
	int segmentCount(void) const;

	void wrap(bool bWrap);
	bool wrap() const;
	void drawEvaluatedCurveSegments(void) const;
	void drawControlPoints(void) const;
	void drawControlPoint(int iCtrlPt) const;
	void drawCurve(void) const;
	void invalidate(void) const;

	void toStream(std::ostream& output_stream) const;
	void fromStream(std::istream& input_stream);

protected:
	void init(const float fStartYValue = 0.0f);
	void reevaluate(void) const;
	// this must be called when a control point is added
	void sortControlPoints(void) const;

	const CurveEvaluator* m_pceEvaluator;

	mutable std::vector<Point> m_ptvCtrlPts;
	mutable std::vector<Point> m_ptvEvaluatedCurvePts;
	mutable bool m_bDirty;

	float m_fMaxX;
	bool m_bWrap;
	static float s_fCtrlPtXEpsilon;
};

std::ostream& operator<<(std::ostream& output_stream, const Curve& curve_data);
std::istream& operator>>(std::istream& input_stream, Curve& curve_data);

#endif // CURVE_H_INCLUDED
