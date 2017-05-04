#ifndef _SUBDIVISION_H_
#define _SUBDIVISION_H_
#include "curveevaluator.h"

using namespace std;

class SubdivisionCurveEvaluator : public CurveEvaluator
{
public:
	SubdivisionCurveEvaluator() {
		avgMask = 0.5;
	}
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
	void setAvgMask(float value) { avgMask = value; }
private:
	float avgMask;// left ratio
};

#endif // !_SUBDIVISION_H_
