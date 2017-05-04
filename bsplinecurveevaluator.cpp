#include "beziercurveevaluator.h"
#include "bsplinecurveevaluator.h"
#include "vec.h"
#include <assert.h>

void BSplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const {

	ptvEvaluatedCurvePts.clear();
	vector<Point> result;

	if (bWrap) {
		result.push_back(Point((ptvCtrlPts.end() - 2)->x - fAniLength, (ptvCtrlPts.end() - 2)->y));
		result.push_back(Point((ptvCtrlPts.end() - 1)->x - fAniLength, (ptvCtrlPts.end() - 1)->y));
		result.insert(result.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		result.push_back(Point((ptvCtrlPts.end())->x - fAniLength, (ptvCtrlPts.end())->y));
		result.push_back(Point((ptvCtrlPts.end() + 1)->x - fAniLength, (ptvCtrlPts.end() + 1)->y));
	}
	else {
		ptvEvaluatedCurvePts.push_back(Point(0, ptvCtrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));

		result.push_back(ptvCtrlPts.front());
		result.push_back(ptvCtrlPts.front());
		result.insert(result.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		result.push_back(ptvCtrlPts.back());
		result.push_back(ptvCtrlPts.back());
	}

	BezierCurveEvaluator bezierCurveEvaluator;
	for (size_t i = 0; i + 3 < result.size(); i++) {
		Vec4d xVal(1.0 * result[i].x + 4.0 * result[i + 1].x + 1.0 * result[i + 2].x + 0.0 * result[i + 3].x,
			0.0 * result[i].x + 4.0 * result[i + 1].x + 2.0 * result[i + 2].x + 0.0 * result[i + 3].x,
			0.0 * result[i].x + 2.0 * result[i + 1].x + 4.0 * result[i + 2].x + 0.0 * result[i + 3].x,
			0.0 * result[i].x + 1.0 * result[i + 1].x + 4.0 * result[i + 2].x + 1.0 * result[i + 3].x);
		Vec4d yVal(1.0 * result[i].y + 4.0 * result[i + 1].y + 1.0 * result[i + 2].y + 0.0 * result[i + 3].y,
			0.0 * result[i].y + 4.0 * result[i + 1].y + 2.0 * result[i + 2].y + 0.0 * result[i + 3].y,
			0.0 * result[i].y + 2.0 * result[i + 1].y + 4.0 * result[i + 2].y + 0.0 * result[i + 3].y,
			0.0 * result[i].y + 1.0 * result[i + 1].y + 4.0 * result[i + 2].y + 1.0 * result[i + 3].y);

		xVal = xVal / 6.0;
		yVal = yVal / 6.0;

		vector<Point> combined;
		for (int j = 0; j < 4; j++)
		{
			combined.push_back(Point(xVal[j], yVal[j]));
		}

		vector<Point> eval;
		bezierCurveEvaluator.evaluateCurve(combined, eval, fAniLength, false);
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.end(), eval.begin(), eval.end() - 2);
	}
}