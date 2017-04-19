#include <FL/fl.h>
#include <FL/fl_draw.h>
#include <algorithm>
#include <float.h>
#ifdef _DEBUG
#include <assert.h>
#endif // _DEBUG

#include "indicatorwindow.h"

const float IndicatorWindow::ks_fIndicatorNotFound = -FLT_MAX;

IndicatorWindow::IndicatorWindow(int x, int y, int w, int h, const char* lable) :
Fl_Double_Window(x, y, w, h, lable), 
m_fMin(0.0f), m_fMax(1.0f),
m_fRangeMarkerMin(0.0f), m_fRangeMarkerMax(0.0f),
m_bRangeMarkerEnabled(false), 
m_fFloatingIndicator(0.0f),
m_iSelectedIndicator(-1)
{
	box(FL_NO_BOX);
}

void IndicatorWindow::draw()
{
	fl_color(255, 255, 255);
	fl_rectf(0, 0, w(), h());


	// draw range
	if (m_bRangeMarkerEnabled) {
		fl_color(200,200,200);
		int min = toWindowX(m_fRangeMarkerMin);
		int max = toWindowX(m_fRangeMarkerMax);
		fl_rectf(min, 0, max - min, h());
	}

	// draw indicators
	fl_color(0, 0, 255);
	for (int i = 0; i < m_fvIndicators.size(); ++i) {
		int x = toWindowX(m_fvIndicators[i]);
		fl_line(x, 0, x, h());
	}

	// draw the floating indicator
	if (m_iSelectedIndicator < 0)
		fl_color(255, 0, 0);
	else
		fl_color(0, 255, 0);
	int x = toWindowX(m_fFloatingIndicator);
	fl_line(x, 0, x, h());

}

int IndicatorWindow::handle(int iEvent)
{
	switch (iEvent) {
	case FL_PUSH:
		switch (Fl::event_button()) {
		case 1: // left button
			{
				// find the indicator and move the floating indicator there
				int x = Fl::event_x();
				float fIndicator = findIndicator(x, 5);
				if (fIndicator != ks_fIndicatorNotFound) {
					floatingIndicator(fIndicator);
					redraw();
				}
			}
			break;
		}
		return 1;

	case FL_RELEASE:
		switch (Fl::event_button()) {
		case 1: // left button
			// do the callback function so that the parent can update other
			// widgets accordingly
			do_callback();
			break;
		}
		return 1;

	default:
		break;
	}

	return Fl_Double_Window::handle(iEvent);
}

void IndicatorWindow::range(float fMin, float fMax)
{
	m_fMin = fMin;
	m_fMax = fMax;
#ifdef _DEBUG
	assert(m_fMax > m_fMin);
#endif // _DEBUG
}

void IndicatorWindow::addIndicator(float f)
{
	m_fvIndicators.push_back(f);
	std::sort(m_fvIndicators.begin(), m_fvIndicators.end());
}

void IndicatorWindow::removeIndicator(float f)
{
	for (std::vector<float>::iterator it = m_fvIndicators.begin(); it != m_fvIndicators.end(); ++it) {
		if (*it == f) {
			m_fvIndicators.erase(it);
			break;
		}
	}
}

void IndicatorWindow::clearIndicators()
{
	m_fvIndicators.clear();
}

void IndicatorWindow::floatingIndicator(float f) 
{
	m_fFloatingIndicator = f;

	m_iSelectedIndicator = -1;
	for (int i = 0; i < m_fvIndicators.size(); ++i) {
		if (m_fFloatingIndicator == m_fvIndicators[i])
			m_iSelectedIndicator = i;
	}

}

float IndicatorWindow::floatingIndicator() const
{
	return m_fFloatingIndicator;
}

bool IndicatorWindow::floatingIndicatorSnapped() const
{
	return (m_iSelectedIndicator >= 0);
}

float IndicatorWindow::findIndicator(int x, int iPickWindowSize) const
{
	int iMinDist = iPickWindowSize;
	float fInd = ks_fIndicatorNotFound;

	for (int i = 0; i < m_fvIndicators.size(); ++i) {
		int iIndX = toWindowX(m_fvIndicators[i]);

		int iDist = abs(x - iIndX);
		if (iDist * 2 <= iPickWindowSize) {
			if (iDist < iMinDist) {
				iMinDist = iDist;
				fInd = m_fvIndicators[i];
			}
		}
	}

	return fInd;
}

bool IndicatorWindow::rangeMarkerEnabled() const
{
	return m_bRangeMarkerEnabled;
}

void IndicatorWindow::rangeMarkerEnabled(bool RangeMarkerEnabled)
{
	m_bRangeMarkerEnabled = RangeMarkerEnabled;
}

void IndicatorWindow::rangeMarkerRange(float fRangeMin, float fRangeMax)
{
	m_fRangeMarkerMin = fRangeMin;
	m_fRangeMarkerMax = fRangeMax;
}

float IndicatorWindow::fromWindowX(int x) const
{
	if (w() > 1)
		return (float)x * (m_fMax - m_fMin) / (float)(w() - 1) + m_fMin;

	return 0.0f;
}

int IndicatorWindow::toWindowX(float f) const
{
	if (w() > 1)
		return (int)((f - m_fMin) * (float)(w() - 1) / (m_fMax - m_fMin) + 0.5f);

	return 0;
}
