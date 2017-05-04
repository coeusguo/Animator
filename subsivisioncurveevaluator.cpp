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
	while (k < 10) {
		if (result.size() == 2)
			break;

		vector<Point> divided;
		divided.push_back(result[0]);
		for (int i = 0; i < result.size() - 1; i++) {
			Point mid = (result[i] + result[i + 1]) / 2;
			divided.push_back(mid);
			divided.push_back(result[i + 1]);
		}

		vector<Point> bin;
		bin.push_back(divided[0]);
		for (int i = 0; i < divided.size() - 1; i++) {
			float x = (divided[i + 1].x - divided[i].x) * avgMask + divided[i].x;
			float y = (divided[i + 1].y - divided[i].y) * avgMask + divided[i].y;
			bin.push_back(Point(x, y));
		}
		bin.push_back(divided.back());
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