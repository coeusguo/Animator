#include "catnullromcurveevaluator.h"

void CatnullRomEvaluator::evaluateCurve(
	const std::vector<Point>& ptvCtrlPts, std::vector<Point>& ptvEvaluatedCurvePts,
	const float & fAniLength, const bool & bWrap) const {

	ptvEvaluatedCurvePts.clear();
	vector<Point> result;
	result.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
	vector<Point> d;//used to store the deritives of the related points
	int size = result.size();

	if (bWrap) {
		//add a phantom point to the end of the control points
		result.push_back(Point(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y));
		size++;
		d.push_back(result[size - 1] - result[size - 2]);//D0,the same as Dn
		for (int i = 1; i < size; i++) {
			if (i == size - 1) {
				d.push_back(result[i] - result[i - 1]);
				break;
			}
			d.push_back((result[i + 1] - result[i - 1])/2.0f);
		}
	}
	else {
		if (size <= 2) {
			ptvEvaluatedCurvePts.assign(result.begin(), result.end());
			ptvEvaluatedCurvePts.push_back(Point(0, result[0].y));
			ptvEvaluatedCurvePts.push_back(Point(fAniLength, result[size - 1].y));
			return;
		}
		else {
			d.push_back(result[1] - result[0]);//D0
			for (int i = 1; i < size; i++) {
				if (i == size - 1) {
					d.push_back(result[i] - result[i - 1]);
					break;
				}
				d.push_back((result[i + 1] - result[i - 1])/2.0f);
			}
		}
	}
	
	//compute the curve
	ptvEvaluatedCurvePts.push_back(result[0]);
	for (int i = 0; i < size - 1; i++) {
		vector<Point> points;
		Point v1 = result[i] + d[i] / 3.0f;
		if (v1.x > result[i + 1].x)
			v1.x = result[i + 1].x - EBSILON;
		Point v2 = result[i + 1] - d[i + 1] / 3.0f;
		if (v2.x < result[i].x)
			v2.x = result[i].x + EBSILON;
		points.push_back(result[i]);
		points.push_back(v1);
		points.push_back(v2);
		points.push_back(result[i + 1]);
		//subdivision
		subdivision(points, fAniLength,false);
		for (int k = 1; k < points.size(); k++)
			ptvEvaluatedCurvePts.push_back(points[k]);
	}

	if (bWrap) {
		for (int i = ptvEvaluatedCurvePts.size() - 1; i >= 0; i--) {
			if (ptvEvaluatedCurvePts[i].x == fAniLength) {
				for (int k = i + 1; k < ptvEvaluatedCurvePts.size(); k++) {
					ptvEvaluatedCurvePts[k].x -= fAniLength;
				}
				ptvEvaluatedCurvePts.push_back(Point(0, ptvEvaluatedCurvePts[i].y));
				break;
			}
		}
	}
	else {
		ptvEvaluatedCurvePts.push_back(Point(0, ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));
	}
}
