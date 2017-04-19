#ifndef modelerui_h
#define modelerui_h

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Message.H>

#include "modelerview.h"
#include "modelerdraw.h"
#include "modelerapp.h"
#include "particleSystem.h"
#include "modeleruiwindows.h"

class ModelerUI : public ModelerUIWindows
{
public:
	typedef void (ValueChangedCallback)();

	ModelerUI();
	void show();
	void addControl(const char* szName, float fMin, float fMax, float fStepSize, float fInitVal);
	void replaceModelerView(ModelerView* pwndNewModelerView);
	void currTime(float fTime);
	float currTime() const;
	void endTime(float fEndTime);
	float endTime() const;
	void playStartTime(float fTime);
	float playStartTime() const;
	void playEndTime(float fTime);
	float playEndTime() const;
	void controlValue(int iControl, float fVal);
	float controlValue(int iControl) const;
	void setValueChangedCallback(ValueChangedCallback* pcbf);
	void animate(bool bAnimate);
	int fps();
	void fps(int fps);
	bool simulate() const;
	void simulate(bool bSimulate);
	void redrawModelerView();
    void autoLoadNPlay();

protected:

	Fl_Box* labelBox(int nBox);
	const Fl_Value_Slider* valueSlider(int iSlider) const;
	Fl_Value_Slider* valueSlider(int iSlider);
	void redrawRulers();
	void activeCurvesChanged();
	void indicatorRangeMarkerRange(float fMin, float fMax);
	bool openAniScript(const char* szFileName);
	
private:

	int m_iCurrControlCount;
	ValueChangedCallback* m_pcbfValueChangedCallback;

	bool m_bAnimating;
	bool m_bSaveMovie;
	int m_iFps;
	float m_fPlayStartTime, m_fPlayEndTime;
	std::string m_strMovieFileName;
	int m_iMovieFrameNum;

	inline void cb_openAniScript_i(Fl_Menu_*, void*);
	static void cb_openAniScript(Fl_Menu_*, void*);
	inline void cb_saveAniScript_i(Fl_Menu_*, void*);
	static void cb_saveAniScript(Fl_Menu_*, void*);
	inline void cb_saveBMPFile_i(Fl_Menu_*, void*);
	static void cb_saveBMPFile(Fl_Menu_*, void*);
	inline void cb_saveMovie_i(Fl_Menu_*, void*);
	static void cb_saveMovie(Fl_Menu_*, void*);
	inline void cb_exit_i(Fl_Menu_*, void*);
	static void cb_exit(Fl_Menu_*, void*);
	inline void cb_hide_i(Fl_Window*, void*);
	static void cb_hide(Fl_Window*, void*);
	inline void cb_normal_i(Fl_Menu_*, void*);
	static void cb_normal(Fl_Menu_*, void*);
	inline void cb_flat_i(Fl_Menu_*, void*);
	static void cb_flat(Fl_Menu_*, void*);
	inline void cb_wireframe_i(Fl_Menu_*, void*);
	static void cb_wireframe(Fl_Menu_*, void*);
	inline void cb_high_i(Fl_Menu_*, void*);
	static void cb_high(Fl_Menu_*, void*);
	inline void cb_medium_i(Fl_Menu_*, void*);
	static void cb_medium(Fl_Menu_*, void*);
	inline void cb_low_i(Fl_Menu_*, void*);
	static void cb_low(Fl_Menu_*, void*);
	inline void cb_poor_i(Fl_Menu_*, void*);
	static void cb_poor(Fl_Menu_*, void*);
	inline void cb_aniLen_i(Fl_Menu_*, void*);
	static void cb_aniLen(Fl_Menu_*, void*);
	inline void cb_fps_i(Fl_Slider*, void*);
	static void cb_fps(Fl_Slider*, void*);
	inline void cb_m_modelerWindow_i(Fl_Window*, void*);
	static void cb_m_modelerWindow(Fl_Window*, void*);
	static void cb_sliders(Fl_Widget* o, void* v);
	inline void cb_browser_i(Fl_Browser*, void*);
	static void cb_browser(Fl_Browser*, void*);
	inline void cb_tab_i(Fl_Tabs*, void*);
	static void cb_tab(Fl_Tabs*, void*);
	inline void cb_graphWidget_i(GraphWidget*, void*);
	static void cb_graphWidget(GraphWidget*, void*);
	inline void cb_zoomAll_i(Fl_Button*, void*);
	static void cb_zoomAll(Fl_Button*, void*);
	inline void cb_curveType_i(Fl_Choice*, void*);
	static void cb_curveType(Fl_Choice*, void*);
	inline void cb_wrap_i(Fl_Light_Button*, void*);
	static void cb_wrap(Fl_Light_Button*, void*);
	inline void cb_indicatorWnd_i(IndicatorWindow*, void*);
	static void cb_indicatorWnd(IndicatorWindow*, void*);
	inline void cb_setCamKeyFrame_i(Fl_Button*, void*);
	static void cb_setCamKeyFrame(Fl_Button*, void*);
	inline void cb_removeCamKeyFrame_i(Fl_Button*, void*);
	static void cb_removeCamKeyFrame(Fl_Button*, void*);
	inline void cb_removeAllCamKeyFrames_i(Fl_Button*, void*);
	static void cb_removeAllCamKeyFrames(Fl_Button*, void*);
	inline void cb_timeSlider_i(Fl_Slider*, void*);
	static void cb_timeSlider(Fl_Slider*, void*);
	inline void cb_playStartSlider_i(Fl_Slider*, void*);
	static void cb_playStartSlider(Fl_Slider*, void*);
	inline void cb_playEndSlider_i(Fl_Slider*, void*);
	static void cb_playEndSlider(Fl_Slider*, void*);
	inline void cb_stepBack_i(Fl_Button*, void*);
	static void cb_stepBack(Fl_Button*, void*);
	inline void cb_play_i(Fl_Button*, void*);
	static void cb_play(Fl_Button*, void*);
	inline void cb_pause_i(Fl_Button*, void*);
	static void cb_pause(Fl_Button*, void*);
	inline void cb_stepForw_i(Fl_Button*, void*);
	static void cb_stepForw(Fl_Button*, void*);
	inline void cb_clearSim_i(Fl_Button*, void*);
	static void cb_clearSim(Fl_Button*, void*);
	inline void cb_simulate_i(Fl_Light_Button*, void*);
	static void cb_simulate(Fl_Light_Button*, void*);
	inline void cb_loop_i(Fl_Light_Button*, void*);
	static void cb_loop(Fl_Light_Button*, void*);
	static void cb_timed(void *); // timed callback for animation
};

#endif

