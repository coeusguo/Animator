#include "CurveEvaluator.h"

float CurveEvaluator::s_fFlatnessEpsilon = 0.00001f;
int CurveEvaluator::s_iSegCount = 16;

CurveEvaluator::~CurveEvaluator(void)
{
}

void CurveEvaluator::subdivision(vector<Point>& points4,float length,bool doublePhantom) const{
	Point v0 = points4[0];
	Point v1 = points4[1];
	Point v2 = points4[2];
	Point v3 = points4[3];

	//the curve is flat enough
	if ((v0.distance(v1) + v1.distance(v2) + v2.distance(v3)) / v0.distance(v3) < 1 + threshold) {
		if (points4[3].x > length + EBSILON) {
			for (int i = 2; i >= 0; i--) {
				if (points4[i].x + EBSILON< length) {
					//linear interpolation
					float y = ((length - points4[i].x) / (points4[i + 1].x - points4[i].x + EBSILON)) * (points4[i + 1].y - points4[i].y) + points4[i].y;
					if (doublePhantom) {//delete the points that smaller than length
						while (points4.size() != i + 1) 
							points4.pop_back();
						points4.push_back(Point(length, y));
					}
					else {
						points4.insert(points4.begin() + i + 1, Point(length, y));
					}
					break;
				}
			}
		}
		if (points4[0].x < EBSILON) {
			for (int i = 1; i < 4; i++) {
				if (points4[i].x > EBSILON) {//delete the points that 
					//linear interpolation
					float y = (-points4[i - 1].x / (points4[i].x - points4[i - 1].x + EBSILON)) * (points4[i].y - points4[i - 1].y) + points4[i - 1].y;
					vector<Point> cpy = points4;//copy the points
					points4.clear();
					points4.push_back(Point(0, y));
					for (int k = i; k < 4; k++) 
						points4.push_back(cpy[k]);
					break;
				}
			}
		}
		return;
	}

	Point v0p((v0.x + v1.x) / 2.0f, (v0.y + v1.y) / 2.0f);
	Point v1p((v1.x + v2.x) / 2.0f, (v1.y + v2.y) / 2.0f);
	Point v2p((v2.x + v3.x) / 2.0f, (v2.y + v3.y) / 2.0f);
	Point v0pp((v0p.x + v1p.x) / 2.0f, (v0p.y + v1p.y) / 2.0f);
	Point v1pp((v1p.x + v2p.x) / 2.0f, (v1p.y + v2p.y) / 2.0f);
	Point qu((v0pp.x + v1pp.x) / 2.0f, (v0pp.y + v1pp.y) / 2.0f);

	vector<Point> point4dLeft;
	vector<Point> point4dRight;
	point4dLeft.push_back(v0);
	point4dLeft.push_back(v0p);
	point4dLeft.push_back(v0pp);
	point4dLeft.push_back(qu);

	point4dRight.push_back(qu);
	point4dRight.push_back(v1pp);
	point4dRight.push_back(v2p);
	point4dRight.push_back(v3);

	subdivision(point4dLeft,length,doublePhantom);
	subdivision(point4dRight,length,doublePhantom);

	points4.clear();
	points4.assign(point4dLeft.begin(), point4dLeft.end());
	for (int i = 1; i < point4dRight.size(); i++)
		points4.push_back(point4dRight[i]);
}
