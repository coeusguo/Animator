#ifndef INDICATORWINDOW_H
#define INDICATORWINDOW_H

#pragma warning(disable : 4786)

#include <vector>
#include <FL/Fl_Double_Window.h>

class IndicatorWindow : public Fl_Double_Window
{
public:
	IndicatorWindow(int x, int y, int w, int h, const char* lable = NULL);

	void draw();
	int handle(int iEvent);

	void range(float fMin, float fMax);

	void addIndicator(float f);
	void removeIndicator(float f);
	void clearIndicators();
	void floatingIndicator(float f);
	float floatingIndicator() const;
	bool floatingIndicatorSnapped() const;

	void rangeMarkerEnabled(bool RangeMarkerEnabled);
	bool rangeMarkerEnabled() const;
	void rangeMarkerRange(float fRangeMin, float fRangeMax);

	float findIndicator(int x, int iPickWindowSize) const;

	static const float ks_fIndicatorNotFound;

protected:
	float fromWindowX(int x) const;
	int toWindowX(float f) const;

	std::vector<float> m_fvIndicators;
	float m_fFloatingIndicator;
	int m_iSelectedIndicator;
	float m_fMin, m_fMax;
	float m_fRangeMarkerMin, m_fRangeMarkerMax;
	bool m_bRangeMarkerEnabled;
};

#endif // INDICATORWINDOW_H