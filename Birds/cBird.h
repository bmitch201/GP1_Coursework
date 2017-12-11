/*
=================
cBird.h
- Header file for class definition - SPECIFICATION
- Header file for the Bird class which is a child of cSprite class
=================
*/
#ifndef _CBIRD_H
#define _CBIRD_H
#include "cSprite.h"
#include "birdGame.h"

class cBird : public cSprite
{
private:
	SDL_Point birdVelocity;

public:
	cBird();

	void update(double deltaTime);		
	void setBirdVelocity(SDL_Point asteroidVel);   // Sets the velocity for the bird
	SDL_Point getBirdVelocity();				 // Gets the bird velocity
};
#endif