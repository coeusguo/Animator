#include <cmath>
#include <string>
#include <float.h>

#include "FL/fl.h"
#include "FL/fl_draw.h"
#include "rulerwindow.h"

RulerWindow::RulerWindow(int x, int y, int w, int h, const char* label) :
Fl_Double_Window(x, y, w, h, label), 
m_byType(RW_HORIZONTAL),
m_dRangeMin(0.0), m_dRangeMax(1.0),
m_iTextSize(12),
m_flcColor(FL_BLACK)
{
}

RulerWindow::~RulerWindow()
{
}

void RulerWindow::type(unsigned char t)
{
	if (t == RW_VERTICAL || t == RW_HORIZONTAL)
		m_byType = t;
}

void RulerWindow::range(double min, double max)
{
	if (min < max) {
		m_dRangeMin = min;
		m_dRangeMax = max;
	}
}

void RulerWindow::textsize(int s)
{
	m_iTextSize = s;
}

// Print a number (iMag * 10^iPow)
static void snprintDecimal(char* szBuffer, const int iBufferCount, const int iMag, const int iPow)
{
	std::string strMag;
	std::string strNumber;

	// Print the magnitude part
	_snprintf(szBuffer, iBufferCount, "%d", iMag);
	szBuffer[iBufferCount - 1] = '\0';

	strMag = szBuffer;
	strNumber = strMag;
	if (iPow >= 0) {
		// Append some zeros
		for (int i = 0; i < iPow; ++i)
			strNumber += "0";
	}
	else { // iPow < 0
		if (-iPow >= strMag.length()) {
			// Add leading zeros
			for (int i = 0; i < -iPow - strMag.length(); ++i)
				strNumber.insert(strNumber.begin(), '0');
			std::string strLZ = "0.";
			strLZ += strNumber;
			strNumber = strLZ;
		}
		else { // -iPow < strMag.length()
			// Simply put a point at the proper place
			char szDot[] = ".";
			strNumber.insert(strMag.length() + iPow, szDot);
		}
	}

	strncpy(szBuffer, strNumber.c_str(), iBufferCount);
	szBuffer[iBufferCount - 1] = '\0';
}

void RulerWindow::draw()
{
	char szText[256];

	if (m_dRangeMax != FLT_MAX && m_dRangeMin != FLT_MIN) {
		switch (m_flcColor) {
		case FL_CYAN:
		case FL_GREEN:
			fl_rectf(0, 0, w(), h(), 0, 0, 0);
			break;
		default:
			fl_rectf(0, 0, w(), h(), 255, 255, 255);
			break;
		}

		double dRange = m_dRangeMax - m_dRangeMin;
		int iWindowLen = m_byType == RW_HORIZONTAL ? w() : h();
		const int k_iAvgLongMarkLen = 15;

		int iLongMarkCount = iWindowLen / k_iAvgLongMarkLen;

		if (iLongMarkCount > 0 && iWindowLen > 0) {
			// Computer the long mark length so that it's 10^i where i is an integer
			double dLongMarkLength = dRange / (double)iLongMarkCount;
			double dLongMarkLengthPow = log10(dLongMarkLength);
			int iLongMarkLengthPow = (int)ceil(dLongMarkLengthPow);
			dLongMarkLength = pow(10.0, (double)iLongMarkLengthPow);

			int iStart = (int)ceil(m_dRangeMin / dLongMarkLength);
			int iMarkPos;

			fl_color(m_flcColor);
			fl_font(FL_HELVETICA, 10);
			do {
				iMarkPos = (int)(((double)iStart * dLongMarkLength - m_dRangeMin) / dRange * (double)iWindowLen + 0.5);

				snprintDecimal(szText, 256, iStart, iLongMarkLengthPow);
				if (m_byType == RW_HORIZONTAL) {
					fl_line(iMarkPos, 0, iMarkPos, h() / 2);
					fl_draw(szText, iMarkPos + 2, h() - 1);
				}
				else {
					fl_line(0, h() - iMarkPos, w() / 2, h() - iMarkPos);
					fl_draw(szText, 2, h() - iMarkPos - 2);
				}

				++iStart;
			} while (iMarkPos < iWindowLen);
		}
	}
	else {
		// disabled
		fl_rectf(0, 0, w(), h(), 128, 128, 128);
	}
}

