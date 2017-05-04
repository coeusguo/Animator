#ifndef _BSPLINECURVE_H_
#define _BSPLINECURVE_H_

#pragma warning(disable : 4786)  

#include "CurveEvaluator.h"

//using namespace std;

class BSplineCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
};

#endif // !_BSPLINECURVE_H_
