#include "subsivisioncurveevaluator.h"

void SubdivisionCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const{

	vector<Point> result;
	result.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
	if (bWrap) {
		result.insert(result.begin(), Point(0, 0));
		result.push_back(Point(fAniLength, 0));
	}

	double k = 0;
	while (k < 30) {
		if (result.size() == 2)
			break;
		vector<Point> bin;
		bin.push_back(result[0]);
		for (int i = 0; i < result.size() - 1; i++) {
			float x = (result[i + 1].x - result[i].x) * avgMask + result[i].x;
			float y = (result[i + 1].y - result[i].y) * avgMask + result[i].y;
			bin.push_back(Point(x, y));
		}
		bin.push_back(result.back());
		result = bin;
		k++;
	}

	ptvEvaluatedCurvePts.clear();
	ptvEvaluatedCurvePts.assign(result.begin(), result.end());

	if (!bWrap) {
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(), Point(0, result[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, result.back().y));
	}
}