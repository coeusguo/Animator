///////////////////////////////////////////////////////////////////////
// Class GraphWidget Implementation
//
// GraphWidget is responsible for curve display. GraphWidget creates 5
// curve evaluators in its constructor. Replace some of them with the
// new curve evaluators.
///////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4786)

#ifdef WIN32
#include <windows.h>
#endif
#ifdef _DEBUG
#include <assert.h>
#endif // _DEBUG
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <float.h>
#include <fstream>

#include "GraphWidget.h"

#include "LinearCurveEvaluator.h"
 

#define LEFT		1
#define MIDDLE		2
#define RIGHT		3

#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define MIDDLE_MOUSE_DOWN	4
#define MIDDLE_MOUSE_DRAG	5
#define MIDDLE_MOUSE_UP		6
#define RIGHT_MOUSE_DOWN	7
#define RIGHT_MOUSE_DRAG	8
#define RIGHT_MOUSE_UP		9
#define SHIFT_LEFT_DOWN		10
#define SHIFT_LEFT_DRAG		11
#define SHIFT_LEFT_UP		12
#define SHIFT_MIDDLE_DOWN	13
#define SHIFT_MIDDLE_DRAG	14
#define SHIFT_MIDDLE_UP		15
#define SHIFT_RIGHT_DOWN	16
#define SHIFT_RIGHT_DRAG	17
#define SHIFT_RIGHT_UP		18
#define CTRL_LEFT_DOWN		19
#define CTRL_LEFT_DRAG		20
#define CTRL_LEFT_UP		21
#define CTRL_MIDDLE_DOWN	22
#define CTRL_MIDDLE_DRAG	23
#define CTRL_MIDDLE_UP		24
#define CTRL_RIGHT_DOWN		25
#define CTRL_RIGHT_DRAG		26
#define CTRL_RIGHT_UP		27
#define ALT_LEFT_DOWN		28
#define ALT_LEFT_DRAG		29
#define ALT_LEFT_UP			30

#define GRID_BACKGROUND_INTENSITY 0.65f
#define CURR_CURVE_INTENSITY 1.0f
#define DIMMED_CURVE_INTENSITY 0.45f
#define GRID_LINE_INTENSITY 0.85f
#define GRID_SHADOW_INTENSITY 0.15f
#define PICK_WINDOW_SIZE 9

// Curve colors. Note that these are consistent with
// Fl_Color so that the browser can show the same
// colors as the curves
static const unsigned char byvvColors[CURVE_COLOR_COUNT][3] = {
	{ 204, 204, 204 }, // gray  So this one is NOT consistent, on account of the updated UI.
	{ 255, 0, 0 }, // red
	{ 0, 255, 0 }, // green
	/* { 255, 255, 0 }, */ // can't use yellow because FLTK will change it to black in the browser
	{ 0, 0, 255 }, // blue
	{ 255, 0, 255 }, // magenta
	{ 0, 255, 255 }, // cyan
};
static const Fl_Color flcvColors[CURVE_COLOR_COUNT] = {
	FL_BLACK,
	FL_RED,
	FL_GREEN,
	FL_BLUE,
	FL_MAGENTA,
	FL_CYAN
};

const static float ks_fViewportMargin = 0.01f;

GraphWidget::GraphWidget(int x, int y, int w, int h, const char *label) :
Fl_Gl_Window(x, y, w, h, label),
m_bHasCtrlPtSelection(false),
m_bHasZoomSelection(false),
m_ivvCurrCtrlPts(),
m_ppceCurveEvaluators(NULL),
m_pcrvvCurves(),
m_ivCurveTypes(),
m_iCurrCurve(-1), 
m_ivActiveCurves(),
m_fEndTime(20.0f),
m_fCurrTime(0.0f),
m_rectCurrViewport(0.0f - ks_fViewportMargin, 1.0f + ks_fViewportMargin, 0.0f - ks_fViewportMargin, 1.0f + ks_fViewportMargin),
m_bPanning(false),
m_bHasEvent(false),
m_bLButtonDown(false),
m_bRButtonDown(false),
m_flcCurrCurve(FL_BLACK)
{
	m_ppceCurveEvaluators = new CurveEvaluator*[CURVE_TYPE_COUNT];

	m_ppceCurveEvaluators[CURVE_TYPE_LINEAR] = new LinearCurveEvaluator();

	// TODO: replace the linear evaluator for one of the three types of curves
	m_ppceCurveEvaluators[CURVE_TYPE_BSPLINE] = new LinearCurveEvaluator();
	m_ppceCurveEvaluators[CURVE_TYPE_BEZIER] = new LinearCurveEvaluator();
	m_ppceCurveEvaluators[CURVE_TYPE_CATMULLROM] = new LinearCurveEvaluator();
	// Note that C2-Interpolating curve is not a requirement
	m_ppceCurveEvaluators[CURVE_TYPE_C2INTERPOLATING] = new LinearCurveEvaluator();

}

GraphWidget::~GraphWidget()
{
	for (int iCurve = 0; iCurve < m_pcrvvCurves.size(); ++iCurve) {
		delete m_pcrvvCurves[iCurve];
	}

	for (int i = 0; i < CURVE_TYPE_COUNT; ++i) {
		delete m_ppceCurveEvaluators[i];
	}
	delete[] m_ppceCurveEvaluators;
}

int GraphWidget::addCurve(const float fStartVal, const float fMinY, const float fMaxY)
{
	Curve* pcrv = new Curve(m_fEndTime, fStartVal);
	pcrv->setEvaluator(m_ppceCurveEvaluators[CURVE_TYPE_LINEAR]);

	m_pcrvvCurves.push_back(pcrv);
	m_cdvCurveDomains.push_back(CurveDomain(fMinY, fMaxY));
	m_ivCurveTypes.push_back(CURVE_TYPE_LINEAR);
	m_ivvCurrCtrlPts.push_back(std::vector<int>());

	return m_pcrvvCurves.size() - 1;
}

void GraphWidget::curveType(int iCurve, int iCurveType)
{
	if (iCurve >= 0 && iCurve < m_pcrvvCurves.size() && 
		iCurveType >= 0 && iCurveType < CURVE_TYPE_COUNT) {
		m_pcrvvCurves[iCurve]->setEvaluator(m_ppceCurveEvaluators[iCurveType]);
		m_ivCurveTypes[iCurve] = iCurveType;
		m_pcrvvCurves[iCurve]->invalidate();
	}
	else {
#ifdef _DEBUG
		assert(0);
#endif // _DEBUG
	}
}

void GraphWidget::currCurveType(int iCurveType)
{
	if (m_iCurrCurve >= 0 && 
		iCurveType >= 0 && iCurveType < CURVE_TYPE_COUNT) {
		m_pcrvvCurves[m_iCurrCurve]->setEvaluator(m_ppceCurveEvaluators[iCurveType]);
		m_ivCurveTypes[m_iCurrCurve] = iCurveType;
		m_pcrvvCurves[m_iCurrCurve]->invalidate();
	}
}

int GraphWidget::currCurveType() const
{
	if (m_iCurrCurve >= 0) {
		return m_ivCurveTypes[m_iCurrCurve];
	}

	return -1;
}

void GraphWidget::draw()
{
	if (!valid()) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glViewport(0, 0, w(), h());

		glClearColor(GRID_BACKGROUND_INTENSITY, GRID_BACKGROUND_INTENSITY, GRID_BACKGROUND_INTENSITY, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glClear(GL_COLOR_BUFFER_BIT);

	glColor3d(0,0,0);
	glBegin(GL_POLYGON);
		glVertex2d( -w(), -h()  );
		glVertex2d( -w(),  h()  );
		glVertex2d(  w(),  h()  );
		glVertex2d(  w(), -h()  );
	glEnd();

/************************************************************************************/

	//The grid.. We're copying this from rulerwindow class.
	//Really to two should have a single reference function.
	double dRangeX = rightTime() - leftTime();
	double dRangeY = (rightTime() - leftTime());
	int iWindowWidth = w();
	int iWindowHeight = h();
	const int k_iAvgLongMarkLen = 15;

	int iLongMarkCountX = iWindowWidth / k_iAvgLongMarkLen;
	int iLongMarkCountY = iWindowHeight / k_iAvgLongMarkLen;

	if (iLongMarkCountX > 0 && iWindowWidth > 0) {
		// Computer the long mark length so that it's 10^i where i is an integer
		double dLongMarkLengthX = dRangeX / (double)iLongMarkCountX;
		double dLongMarkLengthPowX = log10(dLongMarkLengthX);
		int iLongMarkLengthPowX = (int)ceil(dLongMarkLengthPowX);
		dLongMarkLengthX = pow(10.0, (double)iLongMarkLengthPowX);

		double dLongMarkLengthY = dRangeY / (double)iLongMarkCountY;
		double dLongMarkLengthPowY = log10(dLongMarkLengthY);
		int iLongMarkLengthPowY = (int)ceil(dLongMarkLengthPowY);
		dLongMarkLengthY = pow(10.0, (double)iLongMarkLengthPowY);


		int iStartX = (int)ceil(leftTime() / dLongMarkLengthX);

		int iMarkX, iMarkY;
		double x,y;

		glColor3d(1,1,1);
		glPointSize(0.5);
		glBegin(GL_POINTS);

			do {
				iMarkX = 2*(int)(((double)iStartX * dLongMarkLengthX - leftTime()) / dRangeX * (double)iWindowWidth + 0.5) - w();
				x = (double)iMarkX / w();
				
				int iStartY = (int)ceil(leftTime() / dLongMarkLengthY);
				do{
					iMarkY = 2*(int)(((double)iStartY * dLongMarkLengthY - leftTime()) / dRangeY * (double)iWindowHeight + 0.5) - h();
					y = (double)iMarkY / h();

					glVertex2d(x,y);

					++iStartY;
				} while (iMarkY < iWindowHeight);
				
				++iStartX;
			} while (iMarkX < iWindowWidth);
		glEnd();
	}

/************************************************************************************/
	
	if (m_bHasEvent) {
		m_bHasEvent = false;

		switch (m_iEventToDo) {
			case LEFT_MOUSE_DOWN:
				selectAddCtrlPt(m_iMouseX, m_iMouseY);
				break;
			case LEFT_MOUSE_DRAG:
				dragCtrlPt(m_iMouseX, m_iMouseY);
				break;
			case LEFT_MOUSE_UP:
				break;

			case ALT_LEFT_DOWN:
				startCtrlPtSelection(m_iMouseX, m_iMouseY);
				break;
			case ALT_LEFT_DRAG:
				doCtrlPtSelection(m_iMouseX, m_iMouseY);
				break;
			case ALT_LEFT_UP:
				endCtrlPtSelection(m_iMouseX, m_iMouseY);
				break;

			case CTRL_LEFT_DOWN:
				selectCurrCurve(m_iMouseX, m_iMouseY);
				break;

			case SHIFT_LEFT_DOWN:
				removeCtrlPt(m_iMouseX, m_iMouseY);
				break;

			case RIGHT_MOUSE_DOWN:
				break;
			case RIGHT_MOUSE_DRAG:
				doZoom(m_iMouseDX, m_iMouseDY);
				break;
			case RIGHT_MOUSE_UP:
				break;

			case SHIFT_RIGHT_DOWN:
				break;
			case SHIFT_RIGHT_DRAG:
				m_bPanning = true;
				doPan(m_iMouseDX, m_iMouseDY);
				break;
			case SHIFT_RIGHT_UP:
				m_bPanning = false;
				break;

			case CTRL_RIGHT_DOWN:
				startZoomSelection(m_iMouseX, m_iMouseY);
				break;
			case CTRL_RIGHT_DRAG:
				doZoomSelection(m_iMouseX, m_iMouseY);
				break;
			case CTRL_RIGHT_UP:
				endZoomSelection(m_iMouseX, m_iMouseY);
				break;

			default:
				break;
		}

		do_callback();
	}
	drawTimeBar();
	drawActiveCurves();
	drawZoomSelectionMap();
	drawSelectionRect();
}

int GraphWidget::handle(int event)
{
	switch (event) {
	case FL_PUSH:
		m_iMouseX = Fl::event_x();
		m_iMouseY = Fl::event_y();
		switch (Fl::event_button()) {
		case LEFT:
			if (!m_bLButtonDown) {
				if (Fl::event_state(FL_SHIFT))
					m_iEventToDo = SHIFT_LEFT_DOWN;
				else if (Fl::event_state(FL_CTRL))
					m_iEventToDo = CTRL_LEFT_DOWN;
				else if (Fl::event_state(FL_ALT))
					m_iEventToDo = ALT_LEFT_DOWN;
				else
					m_iEventToDo = LEFT_MOUSE_DOWN;
				m_bLButtonDown = true;
				m_bHasEvent = true;
			}
			break;
		case RIGHT:
			if (!m_bRButtonDown) {
				if (Fl::event_state(FL_SHIFT))
					m_iEventToDo = SHIFT_RIGHT_DOWN;
				else if (Fl::event_state(FL_CTRL))
					m_iEventToDo = CTRL_RIGHT_DOWN;
				else
					m_iEventToDo = RIGHT_MOUSE_DOWN;
				m_bRButtonDown = true;
				m_bHasEvent = true;
			}
			break;
		}

		if (m_bHasEvent)
			redraw();
		break;

	case FL_DRAG:
		m_iMouseDX = Fl::event_x() - m_iMouseX;
		m_iMouseDY = Fl::event_y() - m_iMouseY;
		m_iMouseX = Fl::event_x();
		m_iMouseY = Fl::event_y();

		if (m_bLButtonDown) {
			if (Fl::event_state(FL_SHIFT))
				m_iEventToDo = SHIFT_LEFT_DRAG;
			else if (Fl::event_state(FL_CTRL))
				m_iEventToDo = CTRL_LEFT_DRAG;
			else if (Fl::event_state(FL_ALT))
				m_iEventToDo = ALT_LEFT_DRAG;
			else 
				m_iEventToDo = LEFT_MOUSE_DRAG;
			m_bHasEvent = true;
		}
		else if (m_bRButtonDown) {
			if (Fl::event_state(FL_SHIFT))
				m_iEventToDo = SHIFT_RIGHT_DRAG;
			else if (Fl::event_state(FL_CTRL))
				m_iEventToDo = CTRL_RIGHT_DRAG;
			else 
				m_iEventToDo = RIGHT_MOUSE_DRAG;
			m_bHasEvent = true;
		}

		if (m_bHasEvent) {
			redraw();
		}
		break;

	case FL_RELEASE:
		m_iMouseX = Fl::event_x();
		m_iMouseY = Fl::event_y();
		switch (Fl::event_button()) {
		case LEFT:
			if (m_bLButtonDown) {
				if (Fl::event_state(FL_SHIFT))
					m_iEventToDo = SHIFT_LEFT_UP;
				else if (Fl::event_state(FL_CTRL))
					m_iEventToDo = CTRL_LEFT_UP;
				else if (Fl::event_state(FL_ALT))
					m_iEventToDo = ALT_LEFT_UP;
				else
					m_iEventToDo = LEFT_MOUSE_UP;
				m_bLButtonDown = false;
				m_bHasEvent = true;
			}
			break;
		case RIGHT:
			if (m_bRButtonDown) {
				if (Fl::event_state(FL_SHIFT))
					m_iEventToDo = SHIFT_RIGHT_UP;
				else if (Fl::event_state(FL_CTRL))
					m_iEventToDo = CTRL_RIGHT_UP;
				else
					m_iEventToDo = RIGHT_MOUSE_UP;
				m_bRButtonDown = false;
				m_bHasEvent = true;
			}
			break;
		}

		if (m_bHasEvent) {
			redraw();
		}
		break;

	default:
		return Fl_Gl_Window::handle(event);
	}

	return 1;
}

float GraphWidget::topValue() const
{
	if (m_iCurrCurve >= 0) {
		return m_rectCurrViewport.top() * m_cdvCurveDomains[m_iCurrCurve].mag() + 
			m_cdvCurveDomains[m_iCurrCurve].minimum();
	}
	else {
		return FLT_MAX;
	}
}

float GraphWidget::bottomValue() const
{
	if (m_iCurrCurve >= 0) {
		return m_rectCurrViewport.bottom() * m_cdvCurveDomains[m_iCurrCurve].mag() + 
			m_cdvCurveDomains[m_iCurrCurve].minimum();
	}
	else {
		return -FLT_MAX;
	}
}

float GraphWidget::leftTime() const
{
	return m_fEndTime * m_rectCurrViewport.left();
}
	
float GraphWidget::rightTime() const
{
	return m_fEndTime * m_rectCurrViewport.right();
}

float GraphWidget::currTime() const
{
	return m_fCurrTime;
}

void GraphWidget::currTime(float fCurrTime)
{
	if (fCurrTime >= 0.0f && fCurrTime <= m_fEndTime)
		m_fCurrTime = fCurrTime;
	else {
	}
}

float GraphWidget::endTime() const
{
	return m_fEndTime;
}

void GraphWidget::endTime(const float fEndTime)
{
	if (fEndTime > 0.0) {
		m_fEndTime = fEndTime;
		for (int i = 0; i < m_pcrvvCurves.size(); ++i) {
			m_pcrvvCurves[i]->maxX(m_fEndTime);
		}
		invalidateAllCurves();
	}
}

void GraphWidget::scaleTime(const float fScale)
{
	for (int i = 0; i < m_pcrvvCurves.size(); ++i) {
		m_pcrvvCurves[i]->scaleX(fScale);
	}
	invalidateAllCurves();
}

void GraphWidget::selectCurrCurve(const int iMouseX, const int iMouseY)
{
	Point ptMouse(iMouseX, iMouseY);

	for (int i = 0; i < m_ivActiveCurves.size(); ++i) {
		int iCurve = m_ivActiveCurves[i];
		Point ptMouseInCurveCoord = windowToCurve(iCurve, ptMouse);
		Point ptClosestPt;
		m_pcrvvCurves[iCurve]->getClosestPoint(ptMouseInCurveCoord, ptClosestPt);
		Point ptClosestPtInWindowCoord = curveToWindow(iCurve, ptClosestPt);

		// select this curve if the mouse cursor is close enough
		if (fabs(ptClosestPtInWindowCoord.x - ptMouse.x) * 2 <= PICK_WINDOW_SIZE &&
			fabs(ptClosestPtInWindowCoord.y - ptMouse.y) * 2 <= PICK_WINDOW_SIZE) {
			m_iCurrCurve = iCurve;
			return;
		}
	}
}

void GraphWidget::selectAddCtrlPt(const int iMouseX, const int iMouseY)
{
	if (m_iCurrCurve >= 0) {
		Point ptMouse(iMouseX, iMouseY);
		Point ptCtrlPt;
		m_ptDragStart = ptMouse;

		// find the closest control point
		Point ptMouseInCurveCoord = windowToCurve(m_iCurrCurve, ptMouse);
		int iClosestCtrlPt = m_pcrvvCurves[m_iCurrCurve]->getClosestControlPoint(ptMouseInCurveCoord, ptCtrlPt);

		Point ptCtrlPtInWindowCoord = curveToWindow(m_iCurrCurve, ptCtrlPt);

		if (fabs(ptCtrlPtInWindowCoord.x - ptMouse.x) * 2 <= PICK_WINDOW_SIZE &&
			fabs(ptCtrlPtInWindowCoord.y - ptMouse.y) * 2 <= PICK_WINDOW_SIZE) {

			if (std::find(m_ivvCurrCtrlPts[m_iCurrCurve].begin(), 
				m_ivvCurrCtrlPts[m_iCurrCurve].end(), 
				iClosestCtrlPt) != m_ivvCurrCtrlPts[m_iCurrCurve].end()) {
				// the point is one of the currently selected points. do nothing
				return;
			}
			else {
				deselectCtrlPts();
				// select the control point
				m_ivvCurrCtrlPts[m_iCurrCurve].push_back(iClosestCtrlPt);
				return;
			}
		}

		// no control point is near the mouse cursor

		// see if it's a currently selected control point 
		for (int i = 0; i < m_ivActiveCurves.size(); ++i) {
			int iCurve = m_ivActiveCurves[i];

			Point ptMouseInCurveCoord = windowToCurve(iCurve, ptMouse);
			int iClosestCtrlPt = m_pcrvvCurves[iCurve]->getClosestControlPoint(ptMouseInCurveCoord, ptCtrlPt);

			Point ptCtrlPtInWindowCoord = curveToWindow(iCurve, ptCtrlPt);

			if (fabs(ptCtrlPtInWindowCoord.x - ptMouse.x) * 2 <= PICK_WINDOW_SIZE &&
				fabs(ptCtrlPtInWindowCoord.y - ptMouse.y) * 2 <= PICK_WINDOW_SIZE) {

				if (std::find(m_ivvCurrCtrlPts[iCurve].begin(), 
					m_ivvCurrCtrlPts[iCurve].end(), 
					iClosestCtrlPt) != m_ivvCurrCtrlPts[iCurve].end()) {
					// the point is one of the currently selected points. do nothing
					return;
				}
			}
		}

		// add a new control point to the current curve
		m_pcrvvCurves[m_iCurrCurve]->addControlPoint(ptMouseInCurveCoord);
		Point ptDummy;
		deselectCtrlPts();
		m_ivvCurrCtrlPts[m_iCurrCurve].push_back(
			m_pcrvvCurves[m_iCurrCurve]->getClosestControlPoint(ptMouseInCurveCoord, ptDummy));
	}
}

void GraphWidget::removeCtrlPt(const int iMouseX, const int iMouseY)
{
	if (m_iCurrCurve >= 0) {
		Point ptMouse(iMouseX, iMouseY);
		Point ptCtrlPt;

		Point ptMouseInCurveCoord = windowToCurve(m_iCurrCurve, ptMouse);
		int iClosestCtrlPt = m_pcrvvCurves[m_iCurrCurve]->getClosestControlPoint(ptMouseInCurveCoord, ptCtrlPt);

		Point ptCtrlPtInWindowCoord = curveToWindow(m_iCurrCurve, ptCtrlPt);

		if (fabs(ptCtrlPtInWindowCoord.x - ptMouse.x) * 2 <= PICK_WINDOW_SIZE &&
			fabs(ptCtrlPtInWindowCoord.y - ptMouse.y) * 2 <= PICK_WINDOW_SIZE) {
			m_pcrvvCurves[m_iCurrCurve]->removeControlPoint(iClosestCtrlPt);
			deselectCtrlPts();
		}
	}
}

void GraphWidget::dragCtrlPt(const int iMouseX, const int iMouseY)
{
	if (m_ivActiveCurves.size() > 0) {
		Point ptMouse(iMouseX, iMouseY);
		ptMouse.x = max(0.0f, ptMouse.x); 
		ptMouse.x = min((float)(w() - 1), ptMouse.x);
		ptMouse.y = max(0.0f, ptMouse.y); 
		ptMouse.y = min((float)(h() - 1), ptMouse.y);
		for (int i = 0; i < m_ivActiveCurves.size(); ++i) {
			int iCurve = m_ivActiveCurves[i];

			Point ptMouseInCurveCoord = windowToCurve(iCurve, ptMouse);
			Point ptDragStartInCurveCoord = windowToCurve(iCurve, m_ptDragStart);
			Point ptOffset(ptMouseInCurveCoord.x - ptDragStartInCurveCoord.x,

			ptMouseInCurveCoord.y - ptDragStartInCurveCoord.y);

			m_pcrvvCurves[iCurve]->moveControlPoints(m_ivvCurrCtrlPts[iCurve], ptOffset,
				m_cdvCurveDomains[iCurve].minimum(), m_cdvCurveDomains[iCurve].maximum());
		}

		m_ptDragStart = ptMouse;
	}
}

void GraphWidget::deselectCtrlPts()
{
	for (int i = 0; i < m_ivvCurrCtrlPts.size(); ++i)
		m_ivvCurrCtrlPts[i].clear();
}

void GraphWidget::startSelection(const int iMouseX, const int iMouseY)
{
	m_rectSelectionRect.bottomLeft(iMouseX, iMouseY);
	m_rectSelectionRect.topRight(iMouseX, iMouseY);
}

void GraphWidget::doSelection(const int iMouseX, const int iMouseY)
{
	m_rectSelectionRect.topRight(iMouseX, iMouseY);
}

void GraphWidget::endSelection(const int iMouseX, const int iMouseY)
{
	m_rectSelectionRect.topRight(iMouseX, iMouseY);
	m_rectSelectionRect.validate();
}

void GraphWidget::doZoom(const int iMouseDX, const int iMouseDY)
{
	Point ptCenter((m_rectCurrViewport.left() + m_rectCurrViewport.right()) * 0.5f,
		(m_rectCurrViewport.bottom() + m_rectCurrViewport.top()) * 0.5f);

	float fXScaling = 1.0f + (float)-iMouseDX / (float)w();
	float fYScaling = 1.0f + (float)iMouseDY / (float)h();

	Rect rectNew = m_rectCurrViewport;
	rectNew.left((rectNew.left() - ptCenter.x) * fXScaling + ptCenter.x);
	rectNew.right((rectNew.right() - ptCenter.x) * fXScaling + ptCenter.x);
	rectNew.top((rectNew.top() - ptCenter.y) * fYScaling + ptCenter.y);
	rectNew.bottom((rectNew.bottom() - ptCenter.y) * fYScaling + ptCenter.y);

	rectNew.validate();
	if (rectNew.left() < 0.0f - ks_fViewportMargin)
		rectNew.left(0.0f - ks_fViewportMargin);
	if (rectNew.left() > 1.0f + ks_fViewportMargin)
		rectNew.left(1.0f + ks_fViewportMargin);
	if (rectNew.right() < 0.0f - ks_fViewportMargin)
		rectNew.right(0.0f - ks_fViewportMargin);
	if (rectNew.right() > 1.0f + ks_fViewportMargin)
		rectNew.right(1.0f + ks_fViewportMargin);
	if (rectNew.top() < 0.0f - ks_fViewportMargin)
		rectNew.top(0.0f - ks_fViewportMargin);
	if (rectNew.top() > 1.0f + ks_fViewportMargin)
		rectNew.top(1.0f + ks_fViewportMargin);
	if (rectNew.bottom() < 0.0f - ks_fViewportMargin)
		rectNew.bottom(0.0f - ks_fViewportMargin);
	if (rectNew.bottom() > 1.0f + ks_fViewportMargin)
		rectNew.bottom(1.0f + ks_fViewportMargin);

	if (rectNew.width() > 0.0f && rectNew.height() > 0.0f)
		m_rectCurrViewport = rectNew;
}

void GraphWidget::startZoomSelection(const int iMouseX, const int iMouseY)
{
	startSelection(iMouseX, iMouseY);
	m_bHasZoomSelection = true;
}

void GraphWidget::doZoomSelection(const int iMouseX, const int iMouseY)
{
	doSelection(iMouseX, iMouseY);
}

void GraphWidget::endZoomSelection(const int iMouseX, const int iMouseY)
{
	endSelection(iMouseX, iMouseY);

	m_bHasZoomSelection = false;

	if (m_rectSelectionRect.width() > 0.0f && m_rectSelectionRect.height() > 0.0f) {
		Rect rectOldViewport = m_rectCurrViewport;

		m_rectCurrViewport.left(
			rectOldViewport.width() * m_rectSelectionRect.left() / (float)w() + rectOldViewport.left());
		m_rectCurrViewport.right(
			rectOldViewport.width() * m_rectSelectionRect.right() / (float)w() + rectOldViewport.left());
		m_rectCurrViewport.bottom(
			rectOldViewport.height() * ((float)h() - m_rectSelectionRect.top()) / (float)h() + rectOldViewport.bottom());
		m_rectCurrViewport.top(
			rectOldViewport.height() * ((float)h() - m_rectSelectionRect.bottom()) / (float)h() + rectOldViewport.bottom());
	}
}

void GraphWidget::startCtrlPtSelection(const int iMouseX, const int iMouseY)
{
	startSelection(iMouseX, iMouseY);
	m_bHasCtrlPtSelection = true;
}

void GraphWidget::doCtrlPtSelection(const int iMouseX, const int iMouseY)
{
	doSelection(iMouseX, iMouseY);
}

void GraphWidget::endCtrlPtSelection(const int iMouseX, const int iMouseY)
{
	endSelection(iMouseX, iMouseY);

	m_bHasCtrlPtSelection = false;

	deselectCtrlPts();

	if (m_rectSelectionRect.width() > 0.0f && m_rectSelectionRect.height() > 0.0f) {
		if (m_ivActiveCurves.size() > 0) {
			for (int i = 0; i < m_ivActiveCurves.size(); ++i) {
				int iCurve = m_ivActiveCurves[i];

				for (int iCtrlPt = 0; iCtrlPt < m_pcrvvCurves[iCurve]->controlPointCount(); ++iCtrlPt) {
					Point ptCtrlPtWndCoord, ptCtrlPt;
					m_pcrvvCurves[iCurve]->getControlPoint(iCtrlPt, ptCtrlPt);
					ptCtrlPtWndCoord = curveToWindow(iCurve, ptCtrlPt);

					if (ptCtrlPtWndCoord.x >= m_rectSelectionRect.left() && 
						ptCtrlPtWndCoord.x <= m_rectSelectionRect.right() &&
						ptCtrlPtWndCoord.y <= m_rectSelectionRect.top() &&
						ptCtrlPtWndCoord.y >= m_rectSelectionRect.bottom()) {
						m_ivvCurrCtrlPts[iCurve].push_back(iCtrlPt);
					}
				}
			}
		}
	}
}

void GraphWidget::doPan(const int iMouseDX, const int iMouseDY)
{
	float fdx = (float)-iMouseDX * m_rectCurrViewport.width() / (float)w();
	float fdy = (float)iMouseDY * m_rectCurrViewport.height() / (float)h();

	fdx = max(-m_rectCurrViewport.left(), fdx);
	fdx = min(1.0f - m_rectCurrViewport.right(), fdx);
	fdy = max(-m_rectCurrViewport.bottom(), fdy);
	fdy = min(1.0f - m_rectCurrViewport.top(), fdy);

	m_rectCurrViewport.left(m_rectCurrViewport.left() + fdx);
	m_rectCurrViewport.right(m_rectCurrViewport.right() + fdx);
	m_rectCurrViewport.bottom(m_rectCurrViewport.bottom() + fdy);
	m_rectCurrViewport.top(m_rectCurrViewport.top() + fdy);
}

void GraphWidget::zoomAll()
{
	m_rectCurrViewport.bottomLeft(0.0f - ks_fViewportMargin, 0.0f - ks_fViewportMargin);
	m_rectCurrViewport.topRight(1.0f + ks_fViewportMargin, 1.0f + ks_fViewportMargin);
}

void GraphWidget::activateCurve(int iCurve, bool bActive)
{
	if (iCurve >= 0 && iCurve < m_pcrvvCurves.size()) {
		if (!bActive) {
			// deselect all control points because this
			// curve is hidden
			deselectCtrlPts();

			if (iCurve == m_iCurrCurve) 
				m_iCurrCurve = -1;
		}

		if (bActive) {
			std::vector<int>::iterator it = std::find(m_ivActiveCurves.begin(), m_ivActiveCurves.end(), iCurve);
			if (it == m_ivActiveCurves.end()) {
				m_ivActiveCurves.push_back(iCurve);
				std::sort(m_ivActiveCurves.begin(), m_ivActiveCurves.end());
			}
		}
		else { // !bActive
			std::vector<int>::iterator it = std::find(m_ivActiveCurves.begin(), m_ivActiveCurves.end(), iCurve);
			if (it != m_ivActiveCurves.end())
				m_ivActiveCurves.erase(it);
		}

		if (m_ivActiveCurves.size() == 1) {
			m_iCurrCurve = m_ivActiveCurves[0];
		}
		else 
			m_iCurrCurve = -1;
	}
}

int GraphWidget::currCurveWrap() const
{
	if (m_iCurrCurve >= 0) {
		bool bWrap = m_pcrvvCurves[m_iCurrCurve]->wrap();
		return bWrap ? 1 : 0;
	}

	return -1;
}

void GraphWidget::currCurveWrap(bool bWrap)
{
	if (m_iCurrCurve >= 0) {
		m_pcrvvCurves[m_iCurrCurve]->wrap(bWrap);
	}
}

void GraphWidget::wrapCurve(int iCurve, bool bWrap)
{
	m_pcrvvCurves[iCurve]->wrap(bWrap);
}

void GraphWidget::invalidateAllCurves()
{
	for (int i = 0; i < m_pcrvvCurves.size(); ++i)
		m_pcrvvCurves[i]->invalidate();
}

const Curve* GraphWidget::curve(int iCurve) const
{
	return m_pcrvvCurves[iCurve];
}

void GraphWidget::drawActiveCurves() const
{
	for (int i = m_ivActiveCurves.size() - 1; i >= 0; --i) {
		int iColor = i % CURVE_COLOR_COUNT;
		drawCurve(m_ivActiveCurves[i], iColor);
	}
}

void GraphWidget::drawCurve(int iCurve, int iColor) const
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(
		m_fEndTime * m_rectCurrViewport.left(), 
		m_fEndTime * m_rectCurrViewport.right(), 
		m_rectCurrViewport.bottom() * m_cdvCurveDomains[iCurve].mag() + m_cdvCurveDomains[iCurve].minimum(),
		m_rectCurrViewport.top() * m_cdvCurveDomains[iCurve].mag() + m_cdvCurveDomains[iCurve].minimum());

#ifdef _DEBUG
	assert(iColor >= 0 && iColor < CURVE_COLOR_COUNT);
#endif _DEBUG
	glColor3ub(byvvColors[iColor][0], byvvColors[iColor][1], byvvColors[iColor][2]);

	// use a different line width and record the color for the current curve
	if (iCurve == m_iCurrCurve) {
		m_flcCurrCurve = flcvColors[iColor];
		glLineWidth(3.0);
	}
	m_pcrvvCurves[iCurve]->drawEvaluatedCurveSegments();
	if (iCurve == m_iCurrCurve)
		glLineWidth(1.0);
	m_pcrvvCurves[iCurve]->drawControlPoints();

	glColor3f(1.0f, 1.0f, 1.0f); // white
	for (int i = 0; i < m_ivvCurrCtrlPts[iCurve].size(); ++i) {
		m_pcrvvCurves[iCurve]->drawControlPoint(m_ivvCurrCtrlPts[iCurve][i]);
	}


	glPopMatrix();
}

void GraphWidget::drawSelectionRect() const
{
	if (m_bHasCtrlPtSelection || m_bHasZoomSelection) {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		gluOrtho2D(0.0, w(), 0.0, h());
		glColor3d(1.0, 0.0, 0.0);

		glBegin(GL_LINE_LOOP);
			glVertex2f(m_rectSelectionRect.left(), h() - m_rectSelectionRect.bottom());
			glVertex2f(m_rectSelectionRect.left(), h() - m_rectSelectionRect.top());
			glVertex2f(m_rectSelectionRect.right(), h() - m_rectSelectionRect.top());
			glVertex2f(m_rectSelectionRect.right(), h() - m_rectSelectionRect.bottom());
		glEnd();

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
}

void GraphWidget::drawZoomSelectionMap() const
{
	if (m_bPanning) {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		gluOrtho2D(-3.0, 1.1, -3.0, 1.1);

		glColor3d(0.0, 1.0, 0.0);
		glBegin(GL_LINE_LOOP);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(1.0f, 0.0f);
			glVertex2f(1.0f, 1.0f);
			glVertex2f(0.0f, 1.0f);
		glEnd();

		glColor3d(1.0, 0.0, 0.0);
		glBegin(GL_LINE_LOOP);
			glVertex2f(m_rectCurrViewport.left(), m_rectCurrViewport.top());
			glVertex2f(m_rectCurrViewport.right(), m_rectCurrViewport.top());
			glVertex2f(m_rectCurrViewport.right(), m_rectCurrViewport.bottom());
			glVertex2f(m_rectCurrViewport.left(), m_rectCurrViewport.bottom());
		glEnd();

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
}

void GraphWidget::drawTimeBar() const
{
	glColor3f(1.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(m_rectCurrViewport.left(), m_rectCurrViewport.right(), 0.0, 1.0);

	glLineWidth(2.0);

	glBegin(GL_LINES);
		glVertex2f(m_fCurrTime / m_fEndTime, 0.0);
		glVertex2f(m_fCurrTime / m_fEndTime, 1.0);
	glEnd();

	glLineWidth(1.0);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

Point GraphWidget::curveToWindow(int iCurve, const Point& ptCurve) const
{
#ifdef _DEBUG
	assert(iCurve >= 0 && iCurve < m_pcrvvCurves.size());
#endif // _DEBUG

	Point ptWindow;

	ptWindow.x = (ptCurve.x - m_rectCurrViewport.left() * m_fEndTime) / 
		(m_rectCurrViewport.width() * m_fEndTime) * 
		(float)w();

	float fNormalizedCurveY = (ptCurve.y - m_cdvCurveDomains[iCurve].minimum()) / 
		m_cdvCurveDomains[iCurve].mag();
	ptWindow.y = (float)h() - 
		(fNormalizedCurveY - m_rectCurrViewport.bottom()) / 
		m_rectCurrViewport.height() * 
		(float)h();

	return ptWindow;
}

Point GraphWidget::windowToCurve(int iCurve, const Point& ptWindow) const
{
#ifdef _DEBUG
	assert(iCurve >= 0 && iCurve < m_pcrvvCurves.size());
#endif // _DEBUG

	Point ptCurve;

	ptCurve.x = (ptWindow.x / (float)w() * m_rectCurrViewport.width() + m_rectCurrViewport.left()) * m_fEndTime;

	ptCurve.y = (((float)h() - ptWindow.y) / (float)h() * m_rectCurrViewport.height() + m_rectCurrViewport.bottom()) * 
		m_cdvCurveDomains[iCurve].mag() + m_cdvCurveDomains[iCurve].minimum();

	return ptCurve;
}

bool GraphWidget::saveScript(const char* szFileName) const
{
	std::ofstream ofsFile;

	ofsFile.open(szFileName, std::ios::out);
	if (!ofsFile.fail()) {
		ofsFile << m_fEndTime << std::endl;
		ofsFile << m_pcrvvCurves.size() << std::endl;

		for (int i = 0; i < m_pcrvvCurves.size(); ++i) {
			ofsFile << m_ivCurveTypes[i] << std::endl;
			m_pcrvvCurves[i]->toStream(ofsFile);
		}

		return true;
	}

	return false;
}

bool GraphWidget::loadScript(const char* szFileName)
{
	std::ifstream ifsFile;

	ifsFile.open(szFileName, std::ios::in);
	if (!ifsFile.fail()) {
		int iCurveCount;
		float fEndTime;

		ifsFile >> fEndTime;
		if (fEndTime <= 0.0f)
			return false;
		endTime(fEndTime);

		ifsFile >> iCurveCount;

		if (iCurveCount != m_pcrvvCurves.size()) {
#ifdef _DEBUG
			assert(0);
#endif // _DEBUG
			return false;
		}

		for (int i = 0; i < iCurveCount; ++i) {
			int iType;
			ifsFile >> iType;
			curveType(i, iType);
			m_pcrvvCurves[i]->fromStream(ifsFile);
		}

		return true;
	}

	return false;
}

Point GraphWidget::windowToGrid( Point p ) {

	double dRange = rightTime() - leftTime();
	int iWindowWidth = w();
	int iWindowHeight = h();
	const int k_iAvgLongMarkLen = 15;

	int iLongMarkCountX = iWindowWidth / k_iAvgLongMarkLen;
	int iLongMarkCountY = iWindowHeight / k_iAvgLongMarkLen;
	int iMarkX=0, iMarkY=0;

	if (iLongMarkCountX > 0 && iWindowWidth > 0) {
		// Computer the long mark length so that it's 10^i where i is an integer
		double dLongMarkLengthX = dRange / (double)iLongMarkCountX;
		double dLongMarkLengthPowX = log10(dLongMarkLengthX);
		int iLongMarkLengthPowX = (int)ceil(dLongMarkLengthPowX);
		dLongMarkLengthX = pow(10.0, (double)iLongMarkLengthPowX);

		iMarkX = ((int)(((p.x-.5) / ((double)iWindowWidth) ) * dRange ) - leftTime())/dLongMarkLengthX;

		double dLongMarkLengthY = dRange / (double)iLongMarkCountY;
		double dLongMarkLengthPowY = log10(dLongMarkLengthY);
		int iLongMarkLengthPowY = (int)ceil(dLongMarkLengthPowY);
		dLongMarkLengthY = pow(10.0, (double)iLongMarkLengthPowY);

		iMarkY = ((int)(((p.y-.5) / ((double)iWindowHeight) ) * dRange ) - leftTime())/dLongMarkLengthY;

	}

	p.x = iMarkX;
	p.y = iMarkY;
	return p;
}

Point GraphWidget::gridToWindow( Point p ) {

	int iWindowWidth = w();
	int iWindowHeight = h();
	const int k_iAvgLongMarkLen = 15;
	double dRange = rightTime() - leftTime();

	int iLongMarkCountX = iWindowWidth / k_iAvgLongMarkLen;
	double dLongMarkLengthX = dRange / (double)iLongMarkCountX;
	double dLongMarkLengthPowX = log10(dLongMarkLengthX);
	int iLongMarkLengthPowX = (int)ceil(dLongMarkLengthPowX);
	dLongMarkLengthX = pow(10.0, (double)iLongMarkLengthPowX);

	int iLongMarkCountY = iWindowHeight / k_iAvgLongMarkLen;
	double dLongMarkLengthY = dRange / (double)iLongMarkCountY;
	double dLongMarkLengthPowY = log10(dLongMarkLengthY);
	int iLongMarkLengthPowY = (int)ceil(dLongMarkLengthPowY);
	dLongMarkLengthY = pow(10.0, (double)iLongMarkLengthPowY);


	Point val;

	val.x = (int)(((double)p.x * dLongMarkLengthX - leftTime()) / dRange * (double)iWindowWidth + 0.5);
	val.y = (int)(((double)p.y * dLongMarkLengthY - leftTime()) / dRange * (double)iWindowHeight + 0.5);

	return val;
}

