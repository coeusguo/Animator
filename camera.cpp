#include <windows.h>
#include <Fl/gl.h>
#include <gl/glu.h>
#include <fstream>

#include "Camera.h"
#include "Curve.h"
#include "CurveEvaluator.h"
#include "LinearCurveEvaluator.h"

#pragma warning(push)
#pragma warning(disable : 4244)

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502f
#endif 

const float kMouseRotationSensitivity		= 1.0f/90.0f;
const float kMouseTranslationXSensitivity	= 0.03f;
const float kMouseTranslationYSensitivity	= 0.03f;
const float kMouseZoomSensitivity			= 0.08f;

void MakeDiagonal(Mat4f &m, float k)
{
	register int i,j;

	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			m[i][j] = (i==j) ? k : 0.0f;
}

void MakeHScale(Mat4f &m, const Vec3f &s)	
{
	MakeDiagonal(m,1.0f);
	m[0][0] = s[0]; m[1][1] = s[1];	m[2][2] = s[2];
}

void MakeHTrans(Mat4f &m, const Vec3f &s)
{
	MakeDiagonal(m,1.0f);
	m[0][3] = s[0]; m[1][3] = s[1]; m[2][3] = s[2];
}

void MakeHRotX(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[1][1] = cosTheta;
	m[1][2] = -sinTheta;
	m[2][1] = sinTheta;
	m[2][2] = cosTheta;
}

void MakeHRotY(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[2][0] = -sinTheta;
	m[0][2] = sinTheta;
	m[2][2] = cosTheta;
}

void MakeHRotZ(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[0][1] = -sinTheta;
	m[1][0] = sinTheta;
	m[1][1] = cosTheta;
}


/** 
 * Possibly useful fxn that is not currently called.
 * Computes the rigid body transformation to the 
 * camera reference frame specified by the eye, at and
 * up vectors passed in as arguments.
 */
void MakeCamTrans(Mat4f &m, Vec3f &eye, 
					Vec3f &at, Vec3f &up)
{

	// compute normalized vectors
	Vec3f n_up = up;
	n_up.normalize();
	Vec3f k = (at - eye);
	k.normalize();

	// compute orthonormal basis (i, j, k)
	Vec3f i = k ^ n_up;
	Vec3f j = i ^ k;
	i.normalize();
	j.normalize();

	// construct rotation matrix
	m = Mat4f(i[0], i[1], i[2], 0,
			  j[0], j[1], j[2], 0,
			  -k[0], -k[1], -k[2], 0,
			  0,0,0,1);

}


void Camera::calculateViewingTransformParameters() 
{
	// compute new transformation based on
	// user interaction
	Mat4f dollyXform;
	Mat4f azimXform;
	Mat4f elevXform;
	Mat4f twistXform;
	Mat4f originXform;

	Vec3f upVector;

	MakeHTrans(dollyXform, Vec3f(0,0,mDolly));
	MakeHRotY(azimXform, mAzimuth);
	MakeHRotX(elevXform, mElevation);
	MakeDiagonal(twistXform, 1.0f);
	MakeHTrans(originXform, mLookAt);
	
	mPosition = Vec3f(0,0,0);

	mPosition = originXform * (azimXform * (elevXform * (dollyXform * mPosition)));

	if ( fmod(double(mElevation), 2.0*M_PI) < -M_PI/2 || fmod(double(mElevation), 2.0*M_PI) > M_PI/2 )
		mUpVector= Vec3f(0,-1,0);
	else
		mUpVector= Vec3f(0,1,0);

	mDirtyTransform = false;
}

Camera::Camera() 
{
	mAzimuth = mTwist = 0.0f;
	mElevation = 0.7f;
	mDolly = -20.0f;

	mLookAt = Vec3f( 0, 0, 0 );

	mCurrentMouseAction = kActionNone;

	m_bSnapped = false;
	for (int i=AZIMUTH; i<NUM_KEY_CURVES; i++)
		mKeyframes[i] = NULL;

	calculateViewingTransformParameters();

	mNumKeyframes = 0;
}

void Camera::createCurves(float t, float maxX)
{
	mKeyframes[AZIMUTH] = new Curve(maxX, Point(t, mAzimuth));
	mKeyframes[AZIMUTH]->setEvaluator(new LinearCurveEvaluator());
	mKeyframes[ELEVATION] = new Curve(maxX, Point(t, mElevation));
	mKeyframes[ELEVATION]->setEvaluator(new LinearCurveEvaluator());
	mKeyframes[DOLLY] = new Curve(maxX, Point(t, mDolly));
	mKeyframes[DOLLY]->setEvaluator(new LinearCurveEvaluator());
	mKeyframes[LOOKAT_X] = new Curve(maxX, Point(t, mLookAt[0]));
	mKeyframes[LOOKAT_X]->setEvaluator(new LinearCurveEvaluator());
	mKeyframes[LOOKAT_Y] = new Curve(maxX, Point(t, mLookAt[1]));
	mKeyframes[LOOKAT_Y]->setEvaluator(new LinearCurveEvaluator());
	mKeyframes[LOOKAT_Z] = new Curve(maxX, Point(t, mLookAt[2]));
	mKeyframes[LOOKAT_Z]->setEvaluator(new LinearCurveEvaluator());
}

void Camera::deleteCurves()
{
	for (int i=AZIMUTH; i<NUM_KEY_CURVES; i++)  {
		if (mKeyframes[i] != NULL) {
			delete mKeyframes[i];
			mKeyframes[i] = NULL;
		}
	}
}

void Camera::clickMouse( MouseAction_t action, int x, int y )
{
	mCurrentMouseAction = action;
	mLastMousePosition[0] = x;
	mLastMousePosition[1] = y;
}

void Camera::dragMouse( int x, int y )
{
	Vec3f mouseDelta   = Vec3f(x,y,0.0f) - mLastMousePosition;
	mLastMousePosition = Vec3f(x,y,0.0f);

	switch(mCurrentMouseAction)
	{
	case kActionTranslate:
		{
			calculateViewingTransformParameters();

			double xTrack =  -mouseDelta[0] * kMouseTranslationXSensitivity;
			double yTrack =  mouseDelta[1] * kMouseTranslationYSensitivity;

			Vec3f transXAxis = mUpVector ^ (mPosition - mLookAt);
			transXAxis /= sqrt((transXAxis*transXAxis));
			Vec3f transYAxis = (mPosition - mLookAt) ^ transXAxis;
			transYAxis /= sqrt((transYAxis*transYAxis));

			setLookAt(getLookAt() + transXAxis*xTrack + transYAxis*yTrack);
			
			break;
		}
	case kActionRotate:
		{
			float dAzimuth		=   -mouseDelta[0] * kMouseRotationSensitivity;
			float dElevation	=   mouseDelta[1] * kMouseRotationSensitivity;
			
			setAzimuth(getAzimuth() + dAzimuth);
			setElevation(getElevation() + dElevation);

			if (getAzimuth() > M_PI) 
				mAzimuth -= 2.0*M_PI;
			if (getElevation() > M_PI) 
				mElevation -= 2.0*M_PI;

			fprintf(stderr, "az %f, elev %f\n", mAzimuth, mElevation);

			break;
		}
	case kActionZoom:
		{
			float dDolly = -mouseDelta[1] * kMouseZoomSensitivity;
			setDolly(getDolly() + dDolly);
			break;
		}
	case kActionTwist:
		// Not implemented
	default:
		break;
	}

}

void Camera::releaseMouse( int x, int y )
{
	mCurrentMouseAction = kActionNone;
}


void Camera::applyViewingTransform() {
	if( mDirtyTransform )
		calculateViewingTransformParameters();

	// Place the camera at mPosition, aim the camera at
	// mLookAt, and twist the camera such that mUpVector is up
	gluLookAt(	mPosition[0], mPosition[1], mPosition[2],
				mLookAt[0],   mLookAt[1],   mLookAt[2],
				mUpVector[0], mUpVector[1], mUpVector[2]);
}


/** Update camera params based on keyframes **/
void Camera::update(float t)
{
	// do nothing if no keyframes
	if (mNumKeyframes == 0) 
		return;

	// otherwise, update based on curves
	mAzimuth = mKeyframes[AZIMUTH]->evaluateCurveAt(t);
	mElevation = mKeyframes[ELEVATION]->evaluateCurveAt(t);
	mDolly = mKeyframes[DOLLY]->evaluateCurveAt(t);
	mLookAt[0] = mKeyframes[LOOKAT_X]->evaluateCurveAt(t);
	mLookAt[1] = mKeyframes[LOOKAT_Y]->evaluateCurveAt(t);
	mLookAt[2] = mKeyframes[LOOKAT_Z]->evaluateCurveAt(t);

	mDirtyTransform = true;
}


bool Camera::setKeyframe(float t, float maxT)
{
	if (m_bSnapped)
		removeKeyframe(t);

	if (mNumKeyframes == 0) {
		// delete old curves
		deleteCurves();
		createCurves(t, maxT);
		mNumKeyframes++;
		return true;
	}

	// check if we're too close to a ctrl pt
	double TIME_EPSILON = 0.01;
	Point closestCtrlPt;
	Point azimuthPt(t, mAzimuth);
	mKeyframes[AZIMUTH]->getClosestControlPoint(azimuthPt, closestCtrlPt);
	if (fabs(t - closestCtrlPt.x) <= TIME_EPSILON)
		return false;

	mKeyframes[AZIMUTH]->addControlPoint(Point(t, mAzimuth));
	mKeyframes[ELEVATION]->addControlPoint(Point(t, mElevation));
	mKeyframes[DOLLY]->addControlPoint(Point(t, mDolly));
	mKeyframes[LOOKAT_X]->addControlPoint(Point(t, mLookAt[0]));
	mKeyframes[LOOKAT_Y]->addControlPoint(Point(t, mLookAt[1]));
	mKeyframes[LOOKAT_Z]->addControlPoint(Point(t, mLookAt[2]));

	mNumKeyframes++;

	return true;
}


void Camera::removeKeyframe(float t)
{
	// check to see if there's something to remove
	if (mNumKeyframes == 0) return;

	Point tmp;
	Point toRemove[NUM_KEY_CURVES];

	toRemove[AZIMUTH] = Point(t, mAzimuth);
	toRemove[ELEVATION] = Point(t, mElevation);
	toRemove[DOLLY] = Point(t, mDolly);
	toRemove[LOOKAT_X] = Point(t, mLookAt[0]);
	toRemove[LOOKAT_Y] = Point(t, mLookAt[1]);
	toRemove[LOOKAT_Z] = Point(t, mLookAt[2]);

	for (int i=AZIMUTH; i<NUM_KEY_CURVES; i++) {
		int j = mKeyframes[i]->getClosestControlPoint(toRemove[i], tmp);
		mKeyframes[i]->removeControlPoint2(j);
	}

	mNumKeyframes--;

}

bool Camera::saveKeyframes(const char* szFileName) const
{
	std::ofstream ofsFile;

	ofsFile.open(szFileName, std::ios::out);
	if (!ofsFile.fail()) {
		ofsFile << mNumKeyframes << std::endl;
		ofsFile << NUM_KEY_CURVES << std::endl;

		if (mKeyframes[0]) 
			for (int i = 0; i < NUM_KEY_CURVES; ++i) {
				mKeyframes[i]->toStream(ofsFile);
			}

		return true;
	}

	return false;
}

bool Camera::loadKeyframes(const char* szFileName)
{
	std::ifstream ifsFile;

	ifsFile.open(szFileName, std::ios::in);
	if (!ifsFile.fail()) {
		int iCurveCount;
		int iNumKeyframes;

		ifsFile >> iNumKeyframes;
		if (iNumKeyframes <= 0)
			return false;
		mNumKeyframes = iNumKeyframes;

		ifsFile >> iCurveCount;

		if (iCurveCount != NUM_KEY_CURVES) {
			return false;
		}

		deleteCurves();
		createCurves(0.0f, 1.0f);

		for (int i = 0; i < iCurveCount; ++i) {
			mKeyframes[i]->fromStream(ifsFile);
		}

		return true;
	}

	return false;
}

float Camera::keyframeTime(int keyframe) const
{
	if (mKeyframes[0]) {
		Point ptCtrlPt;
		mKeyframes[0]->getControlPoint(keyframe, ptCtrlPt);
		return ptCtrlPt.x;
	}
	return 0.0f;
}

#pragma warning(pop)