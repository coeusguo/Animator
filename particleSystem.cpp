#pragma warning(disable : 4786)

#include "particleSystem.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
	// TODO

}





/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	// TODO

}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
    
	// TODO

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
    
	// TODO

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
    
	// TODO

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{

	// TODO
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{

	// TODO
}





/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{

	// TODO
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{

	// TODO
}





