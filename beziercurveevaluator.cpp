#include "beziercurveevaluator.h"

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const {
	
	
	ptvEvaluatedCurvePts.clear();
	vector<Point> result;

	result.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());//copy

	if (bWrap) {//wrap
		Point phantomPoint(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y);
		result.push_back(phantomPoint);
	}
	ptvEvaluatedCurvePts.clear();
	ptvEvaluatedCurvePts.push_back(*(ptvCtrlPts.begin()));

	int i = 0;
	if (result.size() > 3) {
		for (i = 0; i + 3 < result.size(); i += 3) {//C0 continus
			vector<Point> points4;
			points4.assign(result.begin() + i, result.begin() + i + 4);
			subdivision(points4,fAniLength,false);
			for (int k = 1; k < points4.size(); k++)
				ptvEvaluatedCurvePts.push_back(points4[k]);
		}
	}
	

	if (i < result.size() - 1) {//collect the rest less than 4
		for (int k = i + 1; k < result.size(); k++) {
			if ((result[k].x > fAniLength + EBSILON) && (result[k - 1].x <= fAniLength + EBSILON)) {
				float y = ((fAniLength - result[k - 1].x) / (result[k].x - result[k - 1].x)) * (result[k].y - result[k - 1].y) + result[k - 1].y;
				ptvEvaluatedCurvePts.push_back(Point(fAniLength, y));
			}
			ptvEvaluatedCurvePts.push_back(result[k]);
		}
	}

	if (bWrap) {
		for (int i = ptvEvaluatedCurvePts.size() - 1; i >= 0; i--) {
			if (abs(ptvEvaluatedCurvePts[i].x - fAniLength) < EBSILON ) {
				for (int k = i + 1; k < ptvEvaluatedCurvePts.size(); k++) {
					ptvEvaluatedCurvePts[k].x -= fAniLength;
				}
				ptvEvaluatedCurvePts.push_back(Point(0, ptvEvaluatedCurvePts[i].y));
			}
		}
	}
	else {
		ptvEvaluatedCurvePts.push_back(Point(0, ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));
	}

}

