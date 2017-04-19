#ifndef GRAPHWIDGET_H_INCLUDED
#define GRAPHWIDGET_H_INCLUDED

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <string>
#include <vector>
#ifdef _DEBUG
#include <assert.h>
#endif // _DEBUG

#include "rect.h"
#include "point.h"
#include "curve.h"
#include "curveevaluator.h"

#define CURVE_TYPE_LINEAR 0
#define CURVE_TYPE_BSPLINE 1
#define CURVE_TYPE_BEZIER 2
#define CURVE_TYPE_CATMULLROM 3
#define CURVE_TYPE_C2INTERPOLATING 4
#define CURVE_TYPE_COUNT 5

#define CURVE_COLOR_COUNT 6

class CurveDomain
{
public:
	CurveDomain(const float fMin, const float fMax) {
		domainRange(fMin, fMax);
	}

	float minimum() const { return m_fMin; }
	float maximum() const { return m_fMax; }
	float mag() const { return m_fMax - m_fMin; }
	
	void minimum(const float fMin) { 
		if (fMin < m_fMax)
			m_fMin = fMin;
		else {
#ifdef _DEBUG
			assert(0);
#endif // _DEBUG
		}
	}
	void maximum(const float fMax) { 
		if (fMax > m_fMin)
			m_fMax = fMax; 
		else {
#ifdef _DEBUG
			assert(0);
#endif // _DEBUG
		}
	}
	void domainRange(const float fMin, const float fMax) {
		if (fMin < fMax) {
			m_fMin = fMin;
			m_fMax = fMax;
		}
		else {
#ifdef _DEBUG
			assert(0);
#endif // _DEBUG
		}
	}

protected:

	float m_fMin;
	float m_fMax;
};

typedef std::vector<int> int_vector;

class GraphWidget : public Fl_Gl_Window
{
public:
	GraphWidget(int x, int y, int w, int h, const char *l = 0);
	~GraphWidget();

	float topValue() const;
	float bottomValue() const;
	float leftTime() const;
	float rightTime() const;

	float currTime() const;
	void currTime(float fCurrTime);
	float endTime() const;
	void endTime(const float fEndTime);
	void scaleTime(const float scale_factor);

	int addCurve(const float fStartVal, const float fMinY, const float fMaxY);
	void currCurveType(int iCurveType);
	int currCurveType() const;
	void activateCurve(int iCurve, bool bActive);
	void wrapActiveCurves(bool bWrap);
	void wrapCurve(int iCurve, bool bWrap);
	// -1: invalid, 0: not wrapped, 1: wrapped
	int currCurveWrap() const;
	void currCurveWrap(bool bWrap);
	void invalidateAllCurves();
	// note that this value is evaluated lazily (it's only updated
	// after a redraw.
	Fl_Color currCurveColor() const { return m_flcCurrCurve; }

	const Curve* curve(int iCurve) const;
	bool saveScript(const char* szFileName) const;
	bool loadScript(const char* szFileName);

	void zoomAll();

	Point windowToGrid( Point p ) ;
	Point gridToWindow( Point p ) ;

protected:
	int m_iEventToDo;
	bool m_bHasEvent;
	bool m_bLButtonDown;
	bool m_bRButtonDown;
	bool m_bPanning;
	int m_iMouseX, m_iMouseY, m_iMouseDX, m_iMouseDY;

	// the current viewport (for zoomin). 
	// (left, right, bottom, top) = (0, 1, 0, 1) means that there's no zoomin.
	Rect m_rectCurrViewport;
	// the current zoom/ctrl-pt selection rectangle in window coordinates
	Rect m_rectSelectionRect;
	bool m_bHasCtrlPtSelection;
	bool m_bHasZoomSelection;
	Point m_ptDragStart; // in window coordinates

	int m_iCurrCurve;
	mutable Fl_Color m_flcCurrCurve;
	std::vector<Curve*> m_pcrvvCurves;
	std::vector<CurveDomain> m_cdvCurveDomains;
	std::vector<int> m_ivCurveTypes;
	CurveEvaluator** m_ppceCurveEvaluators;
	std::vector<int> m_ivActiveCurves;
	std::vector<int_vector> m_ivvCurrCtrlPts;
	float m_fEndTime;
	float m_fCurrTime;

	void draw();
	int handle(int event);

	void drawActiveCurves() const;
	void drawCurve(int iCurve, int iColor) const;
	void drawSelectionRect() const;
	void drawZoomSelectionMap() const;
	void drawTimeBar() const;

	void selectCurrCurve(const int iMouseX, const int iMouseY);
	void selectAddCtrlPt(const int iMouseX, const int iMouseY);
	void removeCtrlPt(const int iMouseX, const int iMouseY);
	void dragCtrlPt(const int iMouseX, const int iMouseY);
	void deselectCtrlPts();
	void startSelection(const int iMouseX, const int iMouseY);
	void doSelection(const int iMouseX, const int iMouseY);
	void endSelection(const int iMouseX, const int iMouseY);
	void doZoom(const int iMouseDX, const int iMouseDY);
	void startZoomSelection(const int iMouseX, const int iMouseY);
	void doZoomSelection(const int iMouseX, const int iMouseY);
	void endZoomSelection(const int iMouseX, const int iMouseY);
	void startCtrlPtSelection(const int iMouseX, const int iMouseY);
	void doCtrlPtSelection(const int iMouseX, const int iMouseY);
	void endCtrlPtSelection(const int iMouseX, const int iMouseY);
	void doPan(const int iMouseDX, const int iMouseDY);

	void curveType(int iCurve, int iCurveType);

	Point curveToWindow(int iCurve, const Point& ptCurve) const;
	Point windowToCurve(int iCurve, const Point& ptWindow) const;
};

#endif // GRAPHWIDGET_H_INCLUDED
