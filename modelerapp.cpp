#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerapp.h"
#include "modelerview.h"
#include "modelerui.h"
#include "camera.h"

#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Box.H>

#include <cstring>
#include <cstdio>
#include <cstdlib>

// CLASS ModelerControl METHODS

ModelerControl::ModelerControl() : m_minimum(0.0f), m_maximum(1.0f), m_stepsize(0.1f), m_value(0.0f)
{
}

ModelerControl::ModelerControl(const char* name, float minimum, float maximum, float stepsize, float value)
{
	SetVals(name, minimum, maximum, stepsize, value);
}

ModelerControl::ModelerControl(const ModelerControl &o)
{ 
	operator=(o); 
}

ModelerControl&	ModelerControl::operator=(const ModelerControl &o)
{ 
	if (this != &o) 
		SetVals(o.m_name, o.m_minimum, o.m_maximum, o.m_stepsize, o.m_value); 
	return *this;
}

void ModelerControl::SetVals(const char* name, float minimum, float maximum, float stepsize, float value)
{
	strncpy(m_name,name, 128);
	m_minimum  = minimum;
	m_maximum  = maximum;
	m_stepsize = stepsize;
	m_value    = value;
}


// ****************************************************************************


// Set the singleton initially to a NULL instance
ModelerApplication* ModelerApplication::m_instance = NULL;

// CLASS ModelerApplication METHODS

ModelerApplication* ModelerApplication::Instance()
{
	// Make a new instance if none exists, otherwise, return
	// the existing instance of the ModelerApplication
	return (m_instance) ? (m_instance) : (m_instance = new ModelerApplication());
}

void ModelerApplication::Init(ModelerViewCreator_f createView, 
                              const ModelerControl controls[], unsigned numControls)
{
    int i;

	m_animating   = false;
	m_numControls = numControls;

	DWORD dwBtnFaceColor = GetSysColor(COLOR_BTNFACE);

	// Get consistent background color
	Fl::background(GetRValue(dwBtnFaceColor), 
		GetGValue(dwBtnFaceColor),
		GetBValue(dwBtnFaceColor));

    // ********************************************************
    // Create the FLTK user interface
    // ********************************************************
    
    m_ui = new ModelerUI();
    
    // For each control, add appropriate objects to the user interface
    for (i=0; i<m_numControls; i++)
    {
		m_ui->addControl(controls[i].m_name, 
			controls[i].m_minimum, controls[i].m_maximum,
			controls[i].m_stepsize, controls[i].m_value);
    }

	// Setup value-changed callback
	m_ui->setValueChangedCallback(ModelerApplication::ValueChangedCallback);

	ModelerView* modelerView = createView(0, 0, 100, 100 ,NULL);
	m_ui->replaceModelerView(modelerView);
}

ModelerApplication::~ModelerApplication()
{
    // FLTK handles widget deletion
    delete m_ui;
}

int ModelerApplication::Run()
{
	if (m_numControls == -1)
	{
		fprintf(stderr, "ERROR: ModelerApplication must be initialized before Run()!\n");
		return -1;
	}

    // Just tell FLTK to go for it.
   	Fl::visual( FL_RGB | FL_DOUBLE );
	m_ui->show();
	Fl::add_timeout(0, ModelerApplication::RedrawLoop, NULL);

	// Automatically load animator.ani and animator.ani.cam if they exist
	m_ui->autoLoadNPlay();

	return Fl::run();
}

double ModelerApplication::GetControlValue(int controlNumber)
{
    return m_ui->controlValue(controlNumber);
}

void ModelerApplication::SetControlValue(int controlNumber, double value)
{
    m_ui->controlValue(controlNumber, value);
}

ParticleSystem *ModelerApplication::GetParticleSystem()
{
	return ps;
}

void ModelerApplication::SetParticleSystem(ParticleSystem *s)
{
	ps = s;
}

float ModelerApplication::GetTime()
{
	return m_ui->currTime();
}

int ModelerApplication::GetFps()
{
	return m_ui->fps();
}

bool ModelerApplication::Animating()
{
	return m_animating;
}

void ModelerApplication::ValueChangedCallback()
{

	ModelerApplication *m_app = ModelerApplication::Instance();

	ModelerUI *m_ui = m_app->m_ui;
	float currTime = m_ui->currTime();
	float endTime = m_ui->endTime();
	float playEndTime = m_ui->playEndTime();

	ParticleSystem *ps = m_app->GetParticleSystem();
	
	if (ps != NULL) {
		bool simulating = ps->isSimulate();

		// stop simulation if we're at endTime
		double TIME_EPSILON = 0.05;
		if (simulating && (currTime >= (playEndTime - TIME_EPSILON))) {
			ps->stopSimulation(currTime); 
		} 

		// check to see if we're simulating still
		simulating = ps->isSimulate();
		if (simulating != m_ui->simulate()) {
			// if the psystem is dirty,
			// we need to sync to it
			if (ps->isDirty()) {
				m_ui->simulate(simulating == true);
			}
			// otherwise, we sync the psystem
			// to the ui
			else if (m_ui->simulate()) {
				ps->startSimulation(currTime);
			} else {
				ps->stopSimulation(currTime);
			}
		}
		ps->setDirty(false);
	}

	// update camera position
	m_ui->m_pwndModelerView->m_camera->update(currTime);

	m_ui->redrawModelerView();
}

void ModelerApplication::RedrawLoop(void*)
{
	if (ModelerApplication::Instance()->m_animating)
		ModelerApplication::Instance()->m_ui->redrawModelerView();

	// 1/50 second update is good enough
	Fl::add_timeout(0.025, ModelerApplication::RedrawLoop, NULL);
}