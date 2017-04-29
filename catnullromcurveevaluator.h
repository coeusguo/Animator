#pragma once
#ifndef _CATNULLROM_H_
#define _CATNULLROM_H_
#include "CurveEvaluator.h"
using namespace std;

class CatnullRomEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
};

#endif // !_CATNULLROM_H_
