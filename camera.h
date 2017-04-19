// This camera stuff mostly by ehsu

#ifndef CAMERA_H
#define CAMERA_H

#include "vec.h"
#include "mat.h"
#include "rect.h"
#include "point.h"
#include "curve.h"
#include "curveevaluator.h"
#include <vector>

//==========[ class Camera ]===================================================

typedef enum { kActionNone, kActionTranslate, kActionRotate, kActionZoom, kActionTwist,} MouseAction_t;

enum KeyframeCurves
{ 
    AZIMUTH=0, ELEVATION, DOLLY, LOOKAT_X, LOOKAT_Y, LOOKAT_Z, NUM_KEY_CURVES 
};

class Camera {
    
protected:
    
    float		mElevation;
    float		mAzimuth;
    float		mDolly;
    float		mTwist; // Not implemented yet
    
    Vec3f		mLookAt;
    
    Vec3f		mPosition;
    Vec3f		mUpVector;
    bool		mDirtyTransform;
    
    void calculateViewingTransformParameters();
    
    Vec3f			mLastMousePosition;
    MouseAction_t	mCurrentMouseAction;

	Curve *			mKeyframes[NUM_KEY_CURVES];
	int				mNumKeyframes;
    
    
public:
    
    //---[ Constructors ]----------------------------------
    
    // defaults to (0,0,0) facing down negative z axis
    Camera();
    
    //---[ Settings ]--------------------------------------
    
    inline void setElevation( float elevation ) 
    { 
        // don't want elevation to be negative
        if (elevation<0) elevation+=6.28318530717f;
        mElevation = elevation; mDirtyTransform = true; 
    }
    inline float getElevation() const
    { return mElevation; }
    
    inline void setAzimuth( float azimuth )
    { mAzimuth = azimuth; mDirtyTransform = true; }
    inline float getAzimuth() const
    { return mAzimuth; }
    
    inline void setDolly( float dolly )
    { mDolly = dolly; mDirtyTransform = true; }
    inline float getDolly() const
    { return mDolly; }
    
    inline void setTwist( float twist )
    { mTwist = twist; mDirtyTransform = true; }
    inline float getTwist() const
    { return mTwist; }
    
    inline void setLookAt( const Vec3f &lookAt )
    { mLookAt = lookAt; mDirtyTransform = true; }
    inline Vec3f getLookAt() const
    { return mLookAt; }
    
    //---[ Interactive Adjustment ]------------------------
    // these should be used from a mouse event handling routine that calls
    // the startX method on a mouse down, updateX on mouse move and finally
    // endX on mouse up.
    //-----------------------------------------------------
    void clickMouse( MouseAction_t action, int x, int y );
    void dragMouse( int x, int y );
    void releaseMouse( int x, int y );
    
    //---[ Viewing Transform ]--------------------------------
    void applyViewingTransform();

	//---[ Animation ]-------------------------------------
	void createCurves(float t, float maxX);
	void deleteCurves();
	void update(float t);
	bool setKeyframe(float t, float maxT);
	void removeKeyframe(float t);
	bool m_bSnapped;

	int numKeyframes() const 
	{ return mNumKeyframes; }

	//---[ Save/Load Kerframes ]------------------------------
	bool saveKeyframes(const char* szFileName) const;
	bool loadKeyframes(const char* szFileName);
	float keyframeTime(int keyframe) const;
};

#endif