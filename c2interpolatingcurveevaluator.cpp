#include "c2interpolatingcurveevaluator.h"
#include "matrix.h"

void C2InterPolatingCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
	std::vector<Point>& ptvEvaluatedCurvePts, const float & fAniLength, const bool & bWrap) const {

	ptvEvaluatedCurvePts.clear();
	vector<Point> result;
	result.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
	int size = result.size();

	if (bWrap) {
		Point phantomPointRight(result[0].x + fAniLength, result[0].y);
		Point phantomPointLeft(result[size - 1].x - fAniLength, result[size - 1].y);
		result.insert(result.begin(), phantomPointLeft);
		result.push_back(phantomPointRight);
		size += 2;
	}
	else {
		if (size == 2) {
			ptvEvaluatedCurvePts.assign(result.begin(), result.end());
			ptvEvaluatedCurvePts.push_back(Point(0, result[0].y));
			ptvEvaluatedCurvePts.push_back(Point(fAniLength, result[size - 1].y));
			return;
		}
	}

	float* data = new float[size * size];
	
	for (int i = 0; i < size * size; i++)
		data[i] = 0.0f;
	data[0] = 2.0f;
	data[1] = 1.0f;
	data[size * size - 2] = 1.0f;
	data[size * size - 1] = 2.0f;
	for (int i = 1; i < size - 1; i++) {
		int index = i * size + i - 1;
		data[index] = 1.0f;
		data[index + 1] = 4.0f;
		data[index + 2] = 1.0f;
	}

	float* rhs = new float[size * 2];
	rhs[0] = 3 * (result[1].x - result[0].x);
	rhs[1] = 3 * (result[1].y - result[0].y);
	rhs[size * 2 - 2] = 3 * (result[size - 1].x - result[size - 2].x);
	rhs[size * 2 - 1] = 3 * (result[size - 1].y - result[size - 2].y);
	for (int i = 1; i < size - 1; i ++) {
		rhs[i * 2] = 3 * (result[i + 1].x - result[i - 1].x);
		rhs[i * 2 + 1] = 3 * (result[i + 1].y - result[i - 1].y);
	}
	Mat<float> Naruto(data, size, size);
	Mat<float> Hinata(rhs, size, 2);
	Mat<float> Boruto = Naruto.inverse() * Hinata;//D0 ~ Dm

	//compute the curve
	for (int i = 0; i < size - 1; i++) {
		vector<Point> points;
		Point v1 = result[i] + Point(Boruto.at(i, 0) / 3.0f, Boruto.at(i, 1) / 3.0f);
		
		if (v1.x + EBSILON> result[i + 1].x)
			v1.x = result[i + 1].x - EBSILON;
		Point v2 = result[i + 1] - Point(Boruto.at(i + 1, 0) / 3.0f, Boruto.at(i + 1, 1) / 3.0f);
		if (v2.x < result[i].x + EBSILON)
			v2.x = result[i].x + EBSILON;
		points.push_back(result[i]);
		points.push_back(v1);
		points.push_back(v2);
		points.push_back(result[i + 1]);
		//subdivision
		subdivision(points, fAniLength, true);
		for (int k = 0; k < points.size() - 1; k++)
			if (points[k].x < result[i + 1].x && points[k].x >= result[i].x)
				ptvEvaluatedCurvePts.push_back(points[k]);
		
		if (i == size - 2) {//the last group
			ptvEvaluatedCurvePts.push_back(points.back());
		}
	}

	if (!bWrap) {
		ptvEvaluatedCurvePts.push_back(Point(0, result[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, result.back().y));
	}
}
