/*
=================
cPlane.h
- Header file for class definition - SPECIFICATION
- Header file for the Rocket class which is a child of cSprite class
=================
*/
#ifndef _CPLANE_H
#define _CPLANE_H
#include "cSprite.h"

class cPlane : public cSprite
{
private:
	SDL_Point planeVelocity;

public:
	cPlane();
	void update(double deltaTime);		
	void setPlaneVelocity(SDL_Point rocketVel);   // Sets the velocity for the player
	SDL_Point getPlaneVelocity();				 // Gets the plane's velocity
};
#endif