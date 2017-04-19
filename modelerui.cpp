///////////////////////////////////////////////////////////////////////
// Class ModelerUI Implementation
//
// This is a subclass of class ModelerUIWindows. The FLUID generated
// ModelerUIWindows creates widgets and we need to setup their callback
// functions in this class since programming in FLUID is really a 
// terrible task.
//
// Note that FLUID screws up the resizable settings of the widgets when
// it saves the .fl file. If this happens, edit modeleruiwindows.fl
// with a text editor and make sure that m_pwndGraphWidget, 
// m_pwndHRuler, m_pwndVRuler, and m_pwndIndicator each has a resizable
// tag. Then, open modeleruiwindows.fl with FLUID and click file->
// write code immediately. Don't save the modeleruiwindows.fl,
// otherwise those resizable flags will be gone again.
///////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4786)

#ifdef _DEBUG
#include <assert.h>
#endif _DEBUG
#include <string>
#include <FL/fl_ask.h>

#include "modelerui.h"
#include "camera.h"

using namespace std;

inline void ModelerUI::cb_openAniScript_i(Fl_Menu_*, void*)
{
	char *szFileName = fl_file_chooser("Open Animation Script", "*.ani", NULL);
	if (szFileName) {
		if (openAniScript(szFileName)) {
			// successfully opened
		}
		else {
			fl_alert("Sorry! I can't load the animation script!");
		}
	}
}

void ModelerUI::cb_openAniScript(Fl_Menu_* o, void* v)
{
	((ModelerUI*)(o->parent()->user_data()))->cb_openAniScript_i(o,v);
}

inline void ModelerUI::cb_saveAniScript_i(Fl_Menu_*, void*)
{
	char *szFileName = fl_file_chooser("Save Animation Script As", "*.ani", NULL);
	if (szFileName) {
		string strFileName = szFileName;

		// Append the default extension
		char szExt[_MAX_EXT];
		_splitpath(strFileName.c_str(), NULL, NULL, NULL, szExt);
		if (strlen(szExt) == 0)
			strFileName += ".ani";

		if (m_pwndGraphWidget->saveScript(strFileName.c_str())) {
			// save the camera keyframes
			string strCamKeyframeFileName = strFileName + ".cam";
			m_pwndModelerView->m_curve_camera->saveKeyframes(strCamKeyframeFileName.c_str());
		}
		else {
			fl_alert("Sorry! I can't save the animation script!");
		}
	}
}

void ModelerUI::cb_saveAniScript(Fl_Menu_* o, void* v)
{
	((ModelerUI*)(o->parent()->user_data()))->cb_saveAniScript_i(o,v);
}

inline void ModelerUI::cb_saveBMPFile_i(Fl_Menu_*, void*) 
{
	char *szFileName = NULL;
	szFileName = fl_file_chooser("Save Frame As", "*.bmp", NULL);
	if (szFileName) {
		string strFileName = szFileName;

		// Append the default extension
		char szExt[_MAX_EXT];
		_splitpath(strFileName.c_str(), NULL, NULL, NULL, szExt);
		if (strlen(szExt) == 0)
			strFileName += ".bmp";

		m_pwndModelerView->saveBMP(strFileName.c_str());
		m_pwndModelerView->redraw();
	}
}

void ModelerUI::cb_saveBMPFile(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_saveBMPFile_i(o,v);
}

inline void ModelerUI::cb_saveMovie_i(Fl_Menu_*, void*)
{
	char *szFileName = fl_file_chooser("Save Movie As", "*.bmp", NULL);

	if (szFileName) {
		m_strMovieFileName = szFileName;

		// Remove the .bmp part
		char szExt[_MAX_EXT];
		_splitpath(m_strMovieFileName.c_str(), NULL, NULL, NULL, szExt);
		if (!stricmp(szExt, ".bmp"))
			m_strMovieFileName = m_strMovieFileName.substr(0, m_strMovieFileName.length() - 4);

		m_bSaveMovie = true;
		m_iMovieFrameNum = 0;
		m_psldrFPS->deactivate();
		currTime(m_fPlayStartTime);
		animate(true);
	}
}

void ModelerUI::cb_saveMovie(Fl_Menu_* o, void* v)
{
	((ModelerUI*)(o->parent()->user_data()))->cb_saveMovie_i(o,v);
}

inline void ModelerUI::cb_exit_i(Fl_Menu_*, void*) 
{
	m_pwndMainWnd->hide();
	m_pwndModelerWnd->hide();
}

void ModelerUI::cb_exit(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_exit_i(o,v);
}

inline void ModelerUI::cb_hide_i(Fl_Window*, void*) 
{
	m_pwndMainWnd->hide();
	m_pwndModelerWnd->hide();
}

void ModelerUI::cb_hide(Fl_Window* o, void* v)
{
	((ModelerUI*)(o->user_data()))->cb_hide_i(o,v);
}

inline void ModelerUI::cb_normal_i(Fl_Menu_*, void*) 
{
	setDrawMode(NORMAL);
	m_pwndModelerView->redraw();
}

void ModelerUI::cb_normal(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_normal_i(o,v);
}

inline void ModelerUI::cb_flat_i(Fl_Menu_*, void*) 
{
	setDrawMode(FLATSHADE);
	m_pwndModelerView->redraw();
}

void ModelerUI::cb_flat(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_flat_i(o,v);
}

inline void ModelerUI::cb_wireframe_i(Fl_Menu_*, void*) 
{
	setDrawMode(WIREFRAME);
	m_pwndModelerView->redraw();
}

void ModelerUI::cb_wireframe(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_wireframe_i(o,v);
}

inline void ModelerUI::cb_high_i(Fl_Menu_*, void*) 
{
	setQuality(HIGH);
	m_pwndModelerView->redraw();
}

void ModelerUI::cb_high(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_high_i(o,v);
}

inline void ModelerUI::cb_medium_i(Fl_Menu_*, void*) 
{
	setQuality(MEDIUM);
	m_pwndModelerView->redraw();
}

void ModelerUI::cb_medium(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_medium_i(o,v);
}

inline void ModelerUI::cb_low_i(Fl_Menu_*, void*) 
{
	setQuality(LOW);
	m_pwndModelerView->redraw();
}

void ModelerUI::cb_low(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_low_i(o,v);
}

inline void ModelerUI::cb_poor_i(Fl_Menu_*, void*) 
{
	setQuality(POOR);
	m_pwndModelerView->redraw();
}

void ModelerUI::cb_poor(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_poor_i(o,v);
}

inline void ModelerUI::cb_aniLen_i(Fl_Menu_*, void*) 
{
	int iAniLen;
	const char* szAniLen = NULL;
	do {
		szAniLen = fl_input("New Animation Length (in second) (1 ~ 3600)", "20");

		if (szAniLen) {
			iAniLen = atoi(szAniLen);
			endTime((float)iAniLen);
		}
	} while (szAniLen && (iAniLen < 1 || iAniLen > 3600));
}

void ModelerUI::cb_aniLen(Fl_Menu_* o, void* v) 
{
	((ModelerUI*)(o->parent()->user_data()))->cb_aniLen_i(o,v);
}

inline void ModelerUI::cb_fps_i(Fl_Slider*, void*) 
{
	fps(m_psldrFPS->value());
}

void ModelerUI::cb_fps(Fl_Slider* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_fps_i(o,v);
}

void ModelerUI::cb_sliders(Fl_Widget* o, void* v)
{
	ModelerUI* pui = (ModelerUI*)o->user_data();
	// no need to call the callback function if the animation is
	// playing since the timer callback will do it
	if (!pui->m_bAnimating) {
		if (pui->m_pcbfValueChangedCallback)
			pui->m_pcbfValueChangedCallback();
	}
}

inline void ModelerUI::cb_browser_i(Fl_Browser*, void*) 
{
	int iSelectedIndex = 0;
	string strText;

	for (int i = 0; i < m_iCurrControlCount; ++i) {
		if (m_pbrsBrowser->selected(i + 1)) {
			labelBox(i)->show();
			valueSlider(i)->show();
			m_pwndGraphWidget->activateCurve(i, true);

			// change the text color to be the same as the curve color
			strText = m_pbrsBrowser->text(i + 1);
			strText[2] = '0' + iSelectedIndex;
			m_pbrsBrowser->text(i + 1, strText.c_str());

			if (++iSelectedIndex >= CURVE_COLOR_COUNT + 1)
				iSelectedIndex = 0;
			if (iSelectedIndex == FL_YELLOW)
				++iSelectedIndex; // can't use yellow because FLTK will change it to black
		}
		else {
			labelBox(i)->hide();
			valueSlider(i)->hide();
			m_pwndGraphWidget->activateCurve(i, false);

			// change the text color to black
			strText = m_pbrsBrowser->text(i + 1);
			strText[2] = '0';
			m_pbrsBrowser->text(i + 1, strText.c_str());
		}
	}

	redrawRulers();
	activeCurvesChanged();
	// somehow we need to redraw the entire window so that
	// the sliders will show up if we:
	// 1. switch to tab immediately after the program starts
	// 2. select a bunch of items in the browser window
	m_pwndMainWnd->redraw(); 
	m_pwndGraphWidget->redraw();
}

void ModelerUI::cb_browser(Fl_Browser* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_browser_i(o,v);
}

inline void ModelerUI::cb_tab_i(Fl_Tabs* o, void*) 
{
	if (m_ptabTab->value() != (Fl_Widget*)m_pgrpCurveGroup) {
		// slider ctrl mode
		m_pwndModelerView->camera(CTRL_MODE);
		m_pwndModelerView->redraw();
	} else {
		// curve mode
		m_pwndModelerView->camera(CURVE_MODE);
		m_pwndModelerView->redraw();
	}

	if (m_pcbfValueChangedCallback)
		m_pcbfValueChangedCallback();
}

void ModelerUI::cb_tab(Fl_Tabs* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_tab_i(o,v);
}

inline void ModelerUI::cb_graphWidget_i(GraphWidget*, void*) 
{
	activeCurvesChanged();
	redrawRulers();
	if (m_pcbfValueChangedCallback)
		m_pcbfValueChangedCallback();
}

void ModelerUI::cb_graphWidget(GraphWidget* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_graphWidget_i(o,v);
}

inline void ModelerUI::cb_zoomAll_i(Fl_Button*, void*) 
{
	m_pwndGraphWidget->zoomAll();
	m_pwndGraphWidget->redraw();
	redrawRulers();
}

void ModelerUI::cb_zoomAll(Fl_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_zoomAll_i(o,v);
}

inline void ModelerUI::cb_curveType_i(Fl_Choice*, void*) 
{
	m_pwndGraphWidget->currCurveType(m_pchoCurveType->value());
	m_pwndGraphWidget->redraw();
}

void ModelerUI::cb_curveType(Fl_Choice* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_curveType_i(o,v);
}

inline void ModelerUI::cb_wrap_i(Fl_Light_Button*, void*) 
{
	if (m_pbtWrap->value() == 1) {
		m_pwndGraphWidget->currCurveWrap(true);
	}
	else if (m_pbtWrap->value() == 0) {
		m_pwndGraphWidget->currCurveWrap(false);
	}
	m_pwndGraphWidget->redraw();
}

void ModelerUI::cb_wrap(Fl_Light_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_wrap_i(o,v);
}

inline void ModelerUI::cb_indicatorWnd_i(IndicatorWindow*, void*) 
{
	currTime(m_pwndIndicatorWnd->floatingIndicator());
}

void ModelerUI::cb_indicatorWnd(IndicatorWindow* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_indicatorWnd_i(o,v);
}

inline void ModelerUI::cb_setCamKeyFrame_i(Fl_Button*, void*) 
{
	float fTime = m_psldrTimeSlider->value();
	float fMaxTime = m_psldrTimeSlider->maximum();
	m_pwndIndicatorWnd->floatingIndicator(fTime);

	if (m_pwndModelerView->m_curve_camera->setKeyframe(fTime, fMaxTime)) {
		bool snapped = m_pwndIndicatorWnd->floatingIndicatorSnapped();
		if (!snapped) {
			m_pwndIndicatorWnd->addIndicator(fTime);
			m_pwndIndicatorWnd->floatingIndicator(fTime);
#ifdef _DEBUG
			assert(m_pwndIndicatorWnd->floatingIndicatorSnapped());
#endif _DEBUG
		} 
	}
	m_pwndModelerView->m_curve_camera->m_bSnapped = true;
	m_pbtRemoveCamKeyFrame->activate();
	m_pwndIndicatorWnd->redraw();
		
}

void ModelerUI::cb_setCamKeyFrame(Fl_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_setCamKeyFrame_i(o,v);
}

inline void ModelerUI::cb_removeCamKeyFrame_i(Fl_Button*, void*) 
{
	float fTime = m_psldrTimeSlider->value();
	m_pwndModelerView->m_curve_camera->removeKeyframe(fTime);
	m_pwndIndicatorWnd->removeIndicator(m_pwndIndicatorWnd->floatingIndicator());
	m_psldrTimeSlider->do_callback();
}
void ModelerUI::cb_removeCamKeyFrame(Fl_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_removeCamKeyFrame_i(o,v);
}

inline void ModelerUI::cb_removeAllCamKeyFrames_i(Fl_Button*, void*) 
{
	float fTime = m_psldrTimeSlider->value();
	m_pwndModelerView->m_curve_camera->removeKeyframe(fTime);
	m_pwndIndicatorWnd->clearIndicators();
	m_psldrTimeSlider->do_callback();
}

void ModelerUI::cb_removeAllCamKeyFrames(Fl_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_removeAllCamKeyFrames_i(o,v);
}

inline void ModelerUI::cb_playStartSlider_i(Fl_Slider*, void*) 
{
	playStartTime(m_psldrPlayStart->value());
}

void ModelerUI::cb_playStartSlider(Fl_Slider* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_playStartSlider_i(o,v);
}

inline void ModelerUI::cb_playEndSlider_i(Fl_Slider*, void*) 
{
	playEndTime(m_psldrPlayEnd->value());
}

void ModelerUI::cb_playEndSlider(Fl_Slider* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_playEndSlider_i(o,v);
}

inline void ModelerUI::cb_timeSlider_i(Fl_Slider*, void*) 
{
	currTime(m_psldrTimeSlider->value());
}

void ModelerUI::cb_timeSlider(Fl_Slider* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_timeSlider_i(o,v);
}

inline void ModelerUI::cb_play_i(Fl_Button*, void*) 
{
	m_psldrFPS->deactivate();
	animate(true);
}

void ModelerUI::cb_play(Fl_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_play_i(o,v);
}

inline void ModelerUI::cb_pause_i(Fl_Button*, void*) 
{
	m_psldrFPS->activate();
	animate(false);
}

void ModelerUI::cb_pause(Fl_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_pause_i(o,v);
}

inline void ModelerUI::cb_stepBack_i(Fl_Button*, void*) 
{
	currTime(currTime() - 1.0f / (float)m_iFps);
}

void ModelerUI::cb_stepBack(Fl_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_stepBack_i(o,v);
}

inline void ModelerUI::cb_stepForw_i(Fl_Button*, void*) 
{
	currTime(currTime() + 1.0f / (float)m_iFps);
}

void ModelerUI::cb_stepForw(Fl_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_stepForw_i(o,v);
}

inline void ModelerUI::cb_clearSim_i(Fl_Button* o, void* v)
{
	ParticleSystem* ps = ModelerApplication::Instance()->GetParticleSystem();
	if (ps) {
		ps->clearBaked();
		m_pwndIndicatorWnd->rangeMarkerEnabled(false);
		m_pwndIndicatorWnd->redraw();
	}
}

void ModelerUI::cb_clearSim(Fl_Button* o, void* v)
{
	((ModelerUI*)(o->user_data()))->cb_clearSim_i(o,v);
}

inline void ModelerUI::cb_loop_i(Fl_Light_Button*, void*) 
{
}

void ModelerUI::cb_loop(Fl_Light_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_loop_i(o,v);
}

inline void ModelerUI::cb_simulate_i(Fl_Light_Button* o, void*) 
{
}

void ModelerUI::cb_simulate(Fl_Light_Button* o, void* v) 
{
	((ModelerUI*)(o->user_data()))->cb_simulate_i(o,v);
}

void ModelerUI::cb_timed(void *p)
{
	ModelerUI* pui = (ModelerUI*)p;

	if (!pui->m_bAnimating) 
		return;

	// update UI
	float dt = 1.0 / (float)pui->m_iFps;
	float t = pui->m_pwndGraphWidget->currTime();

	if ((t + dt) > pui->playEndTime()) {
		// stop animating if looping not enabled or
		// if we're saving the movie
		if (!pui->m_pbtLoop->value() || pui->m_bSaveMovie) {
			pui->animate(false);
			pui->currTime(pui->playStartTime());
			return;
		} 
		// otherwise, reset to play start time
		else {
			pui->currTime(pui->playStartTime());
		}
	} 
	else {
		pui->currTime(t + dt);
	}

	Fl::repeat_timeout(dt, cb_timed, (void *)pui);
}

Fl_Box* ModelerUI::labelBox(int nBox) 
{
  return (Fl_Box*)m_ppckPack->child(nBox * 2);
}

Fl_Value_Slider* ModelerUI::valueSlider(int iSlider) 
{
  return (Fl_Value_Slider*)m_ppckPack->child(iSlider * 2 + 1);
}

const Fl_Value_Slider* ModelerUI::valueSlider(int iSlider) const
{
  return (Fl_Value_Slider*)m_ppckPack->child(iSlider * 2 + 1);
}

void ModelerUI::redrawRulers() 
{
	m_pwndHRuler->range(m_pwndGraphWidget->leftTime(), m_pwndGraphWidget->rightTime());
	m_pwndHRuler->redraw();
	m_pwndVRuler->rulercolor(m_pwndGraphWidget->currCurveColor());
	m_pwndVRuler->range(m_pwndGraphWidget->bottomValue(), m_pwndGraphWidget->topValue());
	m_pwndVRuler->redraw();
}

void ModelerUI::activeCurvesChanged()
{
	if (m_pwndGraphWidget->currCurveType() >= 0) {
		m_pchoCurveType->activate();
		m_pchoCurveType->value(m_pwndGraphWidget->currCurveType());
	}
	else
		m_pchoCurveType->deactivate();

	if (m_pwndGraphWidget->currCurveWrap() >= 0) {
		m_pbtWrap->activate();
		m_pbtWrap->value(m_pwndGraphWidget->currCurveWrap());
	}
	else {
		m_pbtWrap->deactivate();
	}
}

void ModelerUI::currTime(float fTime) 
{
	if (fTime < playStartTime())
		fTime = playStartTime();
	if (fTime > playEndTime())
		fTime = playEndTime();

	m_pwndGraphWidget->currTime(fTime);
	m_pwndIndicatorWnd->floatingIndicator(fTime);
	m_psldrTimeSlider->value(fTime);
	m_pwndModelerView->t = fTime;
	
	m_pwndGraphWidget->redraw();
	m_pwndIndicatorWnd->redraw();
	m_psldrTimeSlider->redraw();

	char szTime[64];
	_snprintf(szTime, 64, "%.2f", fTime);
	szTime[63] = 0;
	m_poutTime->value(szTime);
	m_poutTime->redraw();
	
	if (m_pwndIndicatorWnd->floatingIndicatorSnapped()) {
		m_pbtRemoveCamKeyFrame->activate();
		m_pwndModelerView->m_curve_camera->m_bSnapped = true;
	}
	else {
		m_pbtRemoveCamKeyFrame->deactivate();
		m_pwndModelerView->m_curve_camera->m_bSnapped = false;
	}

	// do the extra callback
	if (m_pcbfValueChangedCallback)
		m_pcbfValueChangedCallback();

	if (simulate()) {
		// update indicator window for particle simulation range

		ParticleSystem *ps = ModelerApplication::Instance()->GetParticleSystem();

		if (ps != NULL) {
			float bakeStartTime = ModelerApplication::Instance()->GetParticleSystem()->getBakeStartTime();
			float bakeEndTime = ModelerApplication::Instance()->GetParticleSystem()->getBakeEndTime();
			if (bakeEndTime < 0.0f)
				bakeEndTime = fTime;
			indicatorRangeMarkerRange(bakeStartTime, bakeEndTime);

		}
	}
}

float ModelerUI::currTime() const
{
	return m_pwndGraphWidget->currTime();
}

void ModelerUI::endTime(float fEndTime) 
{
	m_pwndGraphWidget->endTime(fEndTime);
	m_pwndGraphWidget->redraw();
	
	m_psldrPlayStart->range(0.0, fEndTime);
	playStartTime(0.0f);
	m_psldrPlayStart->redraw();

	m_psldrPlayEnd->range(0.0, fEndTime);
	playEndTime(fEndTime);
	m_psldrPlayEnd->redraw();
	
	m_psldrTimeSlider->range(0.0, fEndTime);
	m_psldrTimeSlider->redraw();

	m_pwndIndicatorWnd->range(0.0, fEndTime);
	m_pwndIndicatorWnd->redraw();
}

float ModelerUI::endTime() const
{
	return m_pwndGraphWidget->endTime();
}

void ModelerUI::playStartTime(float fTime)
{
	if (fTime >= 0.0f && fTime <= endTime()) {
		m_fPlayStartTime = fTime;
		m_psldrPlayStart->value(m_fPlayStartTime);
	}

	if (fTime > currTime())
		currTime(fTime);
	if (fTime > playEndTime()) {
		playEndTime(m_fPlayStartTime);
	}

	char szTime[64];
	_snprintf(szTime, 64, "%.2f", fTime);
	szTime[63] = 0;
	m_poutPlayStart->value(szTime);
	m_poutPlayStart->redraw();
}

float ModelerUI::playStartTime() const
{
	return m_fPlayStartTime;
}

void ModelerUI::playEndTime(float fTime)
{
	if (fTime >= 0.0f && fTime <= endTime()) {
		m_fPlayEndTime = fTime;
		m_psldrPlayEnd->value(m_fPlayEndTime);
	}

	if (fTime < currTime())
		currTime(fTime);
	if (fTime < playStartTime()) {
		playStartTime(m_fPlayEndTime);
	}

	char szTime[64];
	_snprintf(szTime, 64, "%.2f", fTime);
	szTime[63] = 0;
	m_poutPlayEnd->value(szTime);
	m_poutPlayEnd->redraw();
}

float ModelerUI::playEndTime() const
{
	return m_fPlayEndTime;
}

float ModelerUI::controlValue(int iControl) const
{
#ifdef _DEBUG
	assert(iControl >= 0 && iControl < m_iCurrControlCount);
#endif _DEBUG

	if (m_ptabTab->value() != (Fl_Widget*)m_pgrpCurveGroup) {
		// slider control mode
		return valueSlider(iControl)->value();
	}
	else {
		// curve mode
		return m_pwndGraphWidget->curve(iControl)->evaluateCurveAt(m_pwndGraphWidget->currTime());
	}
}

void ModelerUI::controlValue(int iControl, float fVal) 
{
	valueSlider(iControl)->value(fVal);
	if (m_pcbfValueChangedCallback)
		m_pcbfValueChangedCallback();
}

void ModelerUI::setValueChangedCallback(ValueChangedCallback* pcbf) 
{
	m_pcbfValueChangedCallback = pcbf;
}

bool ModelerUI::simulate() const
{
	if (m_bAnimating && m_pbtSimulate->value())
		return true;
	return false;
}

void ModelerUI::simulate(bool bSimulate)
{
	if (bSimulate) {
		m_pbtSimulate->set();
	}
	else {
		m_pbtSimulate->clear();
	}
}

void ModelerUI::redrawModelerView()
{
	m_pwndModelerView->redraw();
	// save the frame
	if (m_bSaveMovie) {
		char szFrameNum[128];
		_snprintf(szFrameNum, 128, "%d", m_iMovieFrameNum++);
		std::string strFileName = m_strMovieFileName;
		strFileName += szFrameNum;
		strFileName += ".bmp";
		m_pwndModelerView->saveBMP(strFileName.c_str());
	}
}

void ModelerUI::indicatorRangeMarkerRange(float fMin, float fMax)
{
	m_pwndIndicatorWnd->rangeMarkerRange(fMin, fMax);
	m_pwndIndicatorWnd->rangeMarkerEnabled(true);
}

void ModelerUI::animate(bool bAnimate)
{
	if (bAnimate) {
		m_pbtPlay->label("@||");
		m_pbtPlay->redraw();
		m_pbtPlay->callback((Fl_Callback*)cb_pause);
		m_pbtStepBack->deactivate();
		m_pbtStepForw->deactivate();
		m_pbtClearSim->deactivate();
		m_psldrTimeSlider->deactivate();
		m_psldrPlayStart->deactivate();
		m_psldrPlayEnd->deactivate();
		m_pwndIndicatorWnd->deactivate();

		// if animation is enabled, add timed callback
		Fl::add_timeout(1.0 / (float)m_iFps, cb_timed, (void *)this);
	}
	else {
		m_pbtPlay->label("@>");
		m_pbtPlay->redraw();
		m_pbtPlay->callback((Fl_Callback*)cb_play);
		m_pbtStepBack->activate();
		m_pbtStepForw->activate();
		m_pbtClearSim->activate();
		m_psldrTimeSlider->activate();
		m_psldrPlayStart->activate();
		m_psldrPlayEnd->activate();
		m_pwndIndicatorWnd->activate();

		// otherwise, remove the callback
		Fl::remove_timeout(cb_timed);

		m_bSaveMovie = false;
	}

	m_bAnimating = bAnimate;
}

int ModelerUI::fps()
{
	return m_iFps;
}

void ModelerUI::fps(const int iFps)
{
	m_iFps = iFps;
}

ModelerUI::ModelerUI() : 
m_iCurrControlCount(0), 
m_pcbfValueChangedCallback(NULL),
m_iFps(30),
m_bAnimating(false),
m_bSaveMovie(false)
{
	// setup all the callback functions...
	m_pmiOpenAniScript->callback((Fl_Callback*)cb_openAniScript);
	m_pmiSaveAniScript->callback((Fl_Callback*)cb_saveAniScript);
	m_pmiSaveBitmapFile->callback((Fl_Callback*)cb_saveBMPFile);
	m_pmiSaveMovie->callback((Fl_Callback*)cb_saveMovie);
	m_pmiExit->callback((Fl_Callback*)cb_exit);
	m_pmiNormal->callback((Fl_Callback*)cb_normal);
	m_pmiFlatShaded->callback((Fl_Callback*)cb_flat);
	m_pmiWireFrame->callback((Fl_Callback*)cb_wireframe);
	m_pmiHightQuality->callback((Fl_Callback*)cb_high);
	m_pmiMediumQuality->callback((Fl_Callback*)cb_medium);
	m_pmiLowQuality->callback((Fl_Callback*)cb_low);
	m_pmiPoorQuality->callback((Fl_Callback*)cb_poor);
	m_pmiSetAniLen->callback((Fl_Callback*)cb_aniLen);
	m_pbrsBrowser->callback((Fl_Callback*)cb_browser);
	m_ptabTab->callback((Fl_Callback*)cb_tab);
	m_pwndGraphWidget->callback((Fl_Callback*)cb_graphWidget);
	m_pbtZoomAll->callback((Fl_Callback*)cb_zoomAll);
	m_pchoCurveType->callback((Fl_Callback*)cb_curveType);
	m_pbtWrap->callback((Fl_Callback*)cb_wrap);
	m_pbtSetCamKeyFrame->callback((Fl_Callback*)cb_setCamKeyFrame);
	m_pbtRemoveCamKeyFrame->callback((Fl_Callback*)cb_removeCamKeyFrame);
	m_pbtRemoveAllCamKeyFrames->callback((Fl_Callback*)cb_removeAllCamKeyFrames);
	m_pwndIndicatorWnd->callback((Fl_Callback*)cb_indicatorWnd);
	m_psldrTimeSlider->callback((Fl_Callback*)cb_timeSlider);
	m_psldrPlayStart->callback((Fl_Callback*)cb_playStartSlider);
	m_psldrPlayEnd->callback((Fl_Callback*)cb_playEndSlider);
	m_pbtStepBack->callback((Fl_Callback*)cb_stepBack);
	m_pbtPlay->callback((Fl_Callback*)cb_play);
	m_pbtStepForw->callback((Fl_Callback*)cb_stepForw);
	m_pbtClearSim->callback((Fl_Callback*)cb_clearSim);
	m_pbtLoop->callback((Fl_Callback*)cb_loop);
	m_pbtSimulate->callback((Fl_Callback*)cb_simulate);
	m_psldrFPS->callback((Fl_Callback*)cb_fps);

	m_pwndMainWnd->callback((Fl_Callback*)cb_hide);
	m_pwndMainWnd->when(FL_HIDE);

	m_poutTime->value("0.00");
	m_poutPlayStart->value("0.00");
	m_poutPlayEnd->value("20.00");

	endTime(20.0f);
}

void ModelerUI::show() 
{
	m_pwndMainWnd->resize(30, 30, 
		m_pwndMainWnd->w(), m_pwndMainWnd->h());
	m_pwndMainWnd->show();
	
	m_pwndModelerWnd->resize(40 + m_pwndMainWnd->w(), 30,
		m_pwndModelerWnd->w(), m_pwndModelerWnd->h());
	m_pwndModelerWnd->show();

	m_pwndGraphWidget->do_callback();
}

void ModelerUI::addControl(const char* szName, float fMin, float fMax, float fStepSize, float fInitVal) 
{
	Fl_Group* pgrpCurrBak = Fl_Group::current();
	Fl_Group::current(m_ppckPack);
	
	const int k_iTextHeight = 20;
	const int k_iSliderHeight = 20;

	// Setup the label box
	Fl_Box* box = new Fl_Box(0, 0, m_ppckPack->w(), k_iTextHeight, szName);
	box->labelsize(10);
	box->hide();
	box->box(FL_FLAT_BOX); // otherwise, Fl_Scroll messes up (ehsu)
		
	// Setup the slider
	Fl_Value_Slider *slider = new Fl_Value_Slider(0, 0, m_ppckPack->w(), k_iSliderHeight, 0);
	slider->type(1);
	slider->hide();
	slider->user_data(this);
	slider->callback(cb_sliders);
	slider->range(fMin, fMax);
	slider->step(fStepSize);
	slider->value(fInitVal);

	Fl_Group::current(pgrpCurrBak);

	// Add this entry to the browser
	string strName = "@C0"; // FLTK color encoding, we'll use @C0~@C6
	strName += szName;
	m_pbrsBrowser->add(strName.c_str());
	
	// Setup the curve
	m_pwndGraphWidget->addCurve(fInitVal, fMin, fMax);

	++m_iCurrControlCount;
}

void ModelerUI::replaceModelerView(ModelerView* pwndNewModelerView)
{
	m_pwndModelerWnd->remove(*m_pwndModelerView);
	delete m_pwndModelerView;

	m_pwndModelerView = pwndNewModelerView;
	m_pwndModelerView->resize(0, 0, m_pwndModelerWnd->w(), m_pwndModelerWnd->h());
	m_pwndModelerWnd->add_resizable(*m_pwndModelerView);
}

bool ModelerUI::openAniScript(const char* szFileName)
{
	if (m_pwndGraphWidget->loadScript(szFileName)) {
		endTime(m_pwndGraphWidget->endTime());
		activeCurvesChanged();
		// load the camera keyframes
		string strCamKeyframeFileName = szFileName;
		strCamKeyframeFileName += ".cam";
		m_pwndModelerView->m_curve_camera->loadKeyframes(strCamKeyframeFileName.c_str());
		// sychronize the indicator window with the loaded keyframes
		m_pwndIndicatorWnd->clearIndicators();
		for (int ikf = 0; ikf < m_pwndModelerView->m_curve_camera->numKeyframes(); ++ikf)
			m_pwndIndicatorWnd->addIndicator(m_pwndModelerView->m_curve_camera->keyframeTime(ikf));

		return true;
	}
	else
		return false;
}

void ModelerUI::autoLoadNPlay()
{
	if (openAniScript("animator.ani")) {
		simulate(true);
		animate(true);
	}
}
