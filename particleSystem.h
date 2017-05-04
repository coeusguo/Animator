/***********************
 * ParticleSystem class
 ***********************/

/**
 * The particle system class simply "manages" a collection of particles.
 * Its primary responsibility is to run the simulation, evolving particles
 * over time according to the applied forces using Euler's method.
 * This header file contains the functions that you are required to implement.
 * (i.e. the rest of the code relies on this interface)
 * In addition, there are a few suggested state variables included.
 * You should add to this class (and probably create new classes to model
 * particles and forces) to build your system.
 */

#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__
#define DELTA 0.01
#define EBSILON 0.00001
#include "vec.h"
#include <vector>
#include <FL/gl.h>
using namespace std;

class Particle {
private:
	Vec3f position;
	Vec3f prevPosition;
	Vec3f acc;//m^2/s
	Vec3f normal;
	float mass;
	bool hinata;
	float dampFactor;
	float timeStep;
public:

	void setParticle(Vec3f pos,float mass = 0.001,float damp = 0.01,float timeStep = 0.03333333){
		position = pos;
		prevPosition = pos;
		this->mass = mass;
		hinata = true;
		dampFactor = 1 - damp;
		this->timeStep = timeStep * timeStep;//seconds per frame
	}
	void setTimeStep(float value) { timeStep = value; }
	void addForce(Vec3f f) { acc += f / mass; }

	Vec3f getNormal() { 
		normal.normalize();
		return normal;
	}//normalize the normal and return it

	void update() {
		if (!hinata)
			return;
		Vec3f temp = position;
		position = position + (position - prevPosition) * dampFactor + acc * timeStep;
		prevPosition = temp;
		acc.zeroElements();
	}

	Vec3f& getPosition() { return position; }

	void move(Vec3f dis) {
		if (hinata)
			position += dis;
	}

	void setHinata(bool value) { hinata = value; }
	void updateNormal(Vec3f n) { normal += n; }
	void resetNormal() { normal.zeroElements(); }
};

class Constraint {
private:
	float restDistance;
	Particle* p1;
	Particle* p2;
public:
	Constraint(Particle* p1, Particle* p2) {
		this->p1 = p1;
		this->p2 = p2;
		restDistance = (p1->getPosition() - p2->getPosition()).length();
	}

	void satisfyConstraint() {
		Vec3f dir = p2->getPosition() - p1->getPosition();//p1 to p2
		float distance = dir.length();
		dir.normalize();
		Vec3f correctionHalf = dir * (distance - restDistance) * 0.5;
		p1->move(correctionHalf);
		p2->move(-correctionHalf);
	}
};

class ParticleSystem {

public:



	/** Constructor **/
	ParticleSystem(Vec3f leftPoint, Vec3f rightPoint, double cwidth, double cheight,int numWidth,int numHeight, int fps);


	/** Destructor **/
	virtual ~ParticleSystem();

	/** Simulation fxns **/
	// This fxn should render all particles in the system,
	// at current time t.
	virtual void drawParticles(float t);

	// This fxn should save the configuration of all particles
	// at current time t.
	virtual void bakeParticles(float t);

	// This function should compute forces acting on all particles
	// and update their state (pos and vel) appropriately.
	virtual void computeForcesAndUpdateParticles(float t, bool hasWind);

	// This function should reset the system to its initial state.
	// When you need to reset your simulation, PLEASE USE THIS FXN.
	// It sets some state variables that the UI requires to properly
	// update the display.  Ditto for the following two functions.
	virtual void resetSimulation(float t);

	// This function should start the simulation
	virtual void startSimulation(float t);

	// This function should stop the simulation
	virtual void stopSimulation(float t);

	// This function should clear out your data structure
	// of baked particles (without leaking memory).
	virtual void clearBaked();


	inline int getIndex(float t) {
		return t * bake_fps;//t in seconds * number of frames per second
	}
	// These accessor fxns are implemented for you
	float getBakeStartTime() { return bake_start_time; }
	float getBakeEndTime() { return bake_end_time; }
	float getBakeFps() { return bake_fps; }
	bool isSimulate() { return simulate; }
	bool isDirty() { return dirty; }
	void setDirty(bool d) { dirty = d; }
	void setFps(int value) { bake_fps = value; }
	Particle* getParticle(int x, int y) {
		return particles + y * width + x;
	}
	void resetNormals() {
		for (int i = 0; i < width * height; i++) {
			particles[i].resetNormal();
		}
	}
protected:


	void resetParticlePosition();
	void copy(float t);
	float random();
	/** Some baking-related state **/
	float bake_fps;						// frame rate at which simulation was baked
	float bake_start_time;				// time at which baking started 
										// These 2 variables are used by the UI for
										// updating the grey indicator 
	float bake_end_time;				// time at which baking ended

	/** General state variables **/
	bool simulate;						// flag for simulation mode
	bool dirty;							// flag for updating ui (don't worry about this)

	Vec3f* positionBin;
	Vec3f* normalBin;
	Particle* particles;
	vector<Constraint> constraints;
	int width;//number of particles per row
	int height;//number of particles per column
	float clothWidth;//the width of the cloth
	float clothHeight;
	bool noBakedData;
	Vec3f wind;//force of the wind

	Vec3f leftPosition;
	Vec3f rightPosition;

	Vec3f triNormal(Particle* p1, Particle* p2, Particle* p3) {
		Vec3f dir1 = p2->getPosition() - p1->getPosition();
		Vec3f dir2 = p3->getPosition() - p1->getPosition();
		Vec3f normal = dir1 ^ dir2;
		normal.normalize();
		return normal;
	}
	void addWindForce(Particle* p1, Particle* p2, Particle* p3) {
		Vec3f normal = triNormal(p1, p2, p3);
		Vec3f force = (normal * wind)* normal;
		//cout << "wind force:" << force << endl;
		p1->addForce(force);
		p2->addForce(force);
		p3->addForce(force);
	}
};


#endif	// __PARTICLE_SYSTEM_H__
