#ifndef _C2INTERPOLATING_H_
#define _C2INTERPOLATING_H_
#include "curveevaluator.h"

using namespace std;

class C2InterPolatingCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
};

#endif // !_C2INTERPOLATIING_H_

