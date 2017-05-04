#pragma warning(disable : 4786)

#include "particleSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <random>


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem(Vec3f leftPoint, Vec3f rightPoint,
	double cwidth, double cheight, int numWidth, int numHeight, int fps)
{
	
	width = numWidth;//number of particles per row
	height = numHeight;//number of particles per column
	this->bake_fps = fps;
	this->leftPosition = leftPoint;
	this->rightPosition = rightPoint;
	
	int size = fps * 20;//number of frame in 20 seconds
	particles = new Particle[width * height];
	positionBin = new Vec3f[width * height * size];
	normalBin = new Vec3f[width * height * size];

	clothWidth = cwidth;
	clothHeight = cheight;

	//topleft corner (0,0,0),bottom right (clothWidth,-clothHeight,0);
	float deltaWidth = clothWidth / (width - 1);
	float deltaHeight = clothHeight / (height - 1);
	//loop from top left to bottom right
	for (int Y = 0; Y < height; Y++) {
		for (int X = 0; X < width; X++) {
			int index = Y * width + X;
			Vec3f pos(X * deltaWidth, -Y * deltaHeight, 0);
			//cout << "position" << pos << endl;
			particles[index].setParticle(pos);
			particles[index].setTimeStep(1.0f / bake_fps);
		}
	}

	//make struct and shear constraints
	for (int Y = 0; Y < height; Y++)
	{
		for (int X = 0; X < width; X++)
		{
			if (X < width - 1) 
				constraints.push_back(Constraint(getParticle(X, Y), getParticle(X + 1, Y)));
			if (Y < height - 1)
				constraints.push_back(Constraint(getParticle(X, Y), getParticle(X, Y + 1)));
			if (X < width - 1 && Y < height - 1) {
				constraints.push_back(Constraint(getParticle(X, Y), getParticle(X + 1, Y + 1)));
				constraints.push_back(Constraint(getParticle(X + 1, Y), getParticle(X, Y + 1)));
			}
		}
	}

	//make bend constraints
	for (int Y = 0; Y < height; Y++)
	{
		for (int X = 0; X < width; X++)
		{
			if (X < width - 2) 
				constraints.push_back(Constraint(getParticle(X, Y), getParticle(X + 2, Y)));
			if (Y < height - 2) 
				constraints.push_back(Constraint(getParticle(X, Y), getParticle(X, Y + 2)));
			if (X < width - 2 && Y < height - 2) {
				constraints.push_back(Constraint(getParticle(X, Y), getParticle(X + 2, Y + 2)));
				constraints.push_back(Constraint(getParticle(X + 2, Y), getParticle(X, Y + 2)));
			}
		}
	}

	//fix top left and right point
	particles[0].getPosition() = leftPosition;
	//cout << "fixed left point:" << particles[0].getPosition() << endl;
	particles[0].setHinata(false);
	particles[width - 1].getPosition() = rightPosition;
	//cout << "fixed right point:" << particles[width - 1].getPosition() << endl;
	particles[width - 1].setHinata(false);
	
	//cout << "nani" << endl;
	//satisfy constraint and let free drop 
	resetParticlePosition();
	//set up the bake pool
	clearBaked();
}





/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	delete[]particles;
	delete[]positionBin;
	delete[]normalBin;
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
    
	//set bake_start_time to current time
	bake_start_time = t;
	//cout << "start simulation" << t << endl;
	//set up the the random wind direction and wind force
	//the direction is parallel to the xz plane
	float x = rand() - 1;
	float z = rand() - 1;
	wind = Vec3f(-x, 0, -z);
	wind.normalize();
	wind = wind *  0.000096;//in newton

	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
    //set the bake_end_time to current time
	bake_end_time = t;

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

void ParticleSystem::resetParticlePosition() {
	for (int i = 0; i < 90; i++)
		computeForcesAndUpdateParticles(0, false);
}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
    
	//1.reset the particle system to their original position
	resetParticlePosition();
	//2.reset the bake_start_time and bake_end_time
	bake_start_time = bake_end_time;
	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t,bool hasWind)
{
	if (hasWind) {
		//add the wind force to every particles
		for (int Y = 0; Y < height - 1; Y++) {
			for (int X = 0; X < width - 1; X++) {
				Particle* p11 = particles + Y * width + X;
				Particle* p12 = particles + Y * width + X + 1;
				Particle* p21 = particles + (Y + 1) * width + X;
				Particle* p22 = particles + (Y + 1) * width + X + 1;
				addWindForce(p11, p12, p21);
				addWindForce(p12, p21, p22);
			}
		}
	}

	//add gravity to every particles
	Vec3f gravity(0, -0.00033, 0);
	for (int i = 0; i < width * height; i++)
		particles[i].addForce(gravity);
	//satisfy the constraint
	for (int i = 0; i < 10; i++) {
		for (Constraint c : constraints)
			c.satisfyConstraint();
	}

	for (int i = 0; i < width * height; i++) {
		//cout << "before update:" << particles[i].getPosition() << "  ";
		particles[i].update();
		//cout << "after update:" << particles[i].getPosition() << endl;
	}
	
}

//may need this function
float ParticleSystem::random() {
	//set up the random variable
	uniform_real_distribution<double> rand(0, 2);
	static default_random_engine re;
	return rand(re);
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	//not simulate and outside the baked time interval,no particles to draw
	bool beforeBakedTime = t < bake_start_time;
	bool afterBakedTime = t > bake_end_time && bake_end_time != -1;
	bool justReseted = bake_start_time == bake_end_time;
	//current time doesn`t have baked particles available
	if (!simulate && (beforeBakedTime || afterBakedTime || justReseted))
		return;

	//now simulate button is on,need to rebake the particles on current time t
	if (simulate) {
		bakeParticles(t);
	}
	if (noBakedData)
		return;

	int index = getIndex(t);
	int size = bake_fps * 20;
	
	//reset the normals
	

	for (int Y = 0; Y < height - 1; Y++) {
		for (int X = 0; X < width - 1; X++) {
			//four points of two triangles
			int id1 = (Y * width + X) * size + index;//p11

			int id2 = ((Y + 1) * width + X) * size + index;//p21

			int id3 = (Y * width + X + 1) * size + index;//p12

			int id4 = ((Y + 1) * width + X + 1) * size + index;//p22

			glBegin(GL_TRIANGLE_STRIP);
			//get the position from baking data structure
			glNormal3fv(normalBin[id1].getPointer());
			glVertex3fv(positionBin[id1].getPointer());

			glNormal3fv(normalBin[id2].getPointer());
			glVertex3fv(positionBin[id2].getPointer());

			glNormal3fv(normalBin[id3].getPointer());
			glVertex3fv(positionBin[id3].getPointer());

			glNormal3fv(normalBin[id4].getPointer());
			glVertex3fv(positionBin[id4].getPointer());
			glEnd();
		}
	}
}





/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{
	//1.compute force and update particles
	computeForcesAndUpdateParticles(t,true);
	//2.copy the result to baking data structure
	copy(t);
	noBakedData = false;
}

void ParticleSystem::copy(float t) {

	resetNormals();
	for (int Y = 0; Y < height - 1; Y++) {
		for (int X = 0; X < width - 1; X++) {
			Particle* p11 = particles + Y * width + X;
			Particle* p12 = particles + Y * width + X + 1;
			Particle* p21 = particles + (Y + 1) * width + X;
			Particle* p22 = particles + (Y + 1) * width + X + 1;
			Vec3f normal1 = triNormal(p11, p12, p21);
			Vec3f normal2 = triNormal(p12, p21, p22);
			p11->updateNormal(normal1);
			p12->updateNormal(normal1 + normal2);
			p21->updateNormal(normal1 + normal2);
			p22->updateNormal(normal2);
		}
	}

	int index = getIndex(t);
	int size = bake_fps * 20;
	for (int Y = 0; Y < height; Y++) {
		for (int X = 0; X < width; X++) {
			int idP = Y * width + X;
			int id = idP * size + index;
			positionBin[id] = particles[idP].getPosition();
			normalBin[id] = particles[idP].getNormal();
		}
	}
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
	int size = bake_fps * 20;
	for (int Y = 0; Y < height; Y++) {
		for (int X = 0; X < width; X++) {
			for (int k = 0; k < size; k++) {
				int index = (Y * width + X) * size + k;
				positionBin[index] = Vec3f(-100, -100, -100);
			}
		}
	}
	noBakedData = true;
}





