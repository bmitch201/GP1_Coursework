/*
=================
cBird.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cBird.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cBird::cBird() : cSprite()
{
	this->birdVelocity = { 0, 0 };
}

/*
=================================================================
Update the sprite position
=================================================================
*/

void cBird::update(double deltaTime)
{
	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += this->getSpriteTranslation().x * deltaTime * birdVelocity.x;
	currentSpritePos.y += this->getSpriteTranslation().y * deltaTime * birdVelocity.y;

	this->setSpritePos({ currentSpritePos.x, currentSpritePos.y });
	cout << "Bird position - x: " << this->getSpritePos().x << " y: " << this->getSpritePos().y << " deltaTime: " << deltaTime << endl;
	this->setBoundingRect(this->getSpritePos());

	//Bird Boundaries for the left and right
	if (currentSpritePos.x > 1000 - 65 | currentSpritePos.x < -25)
	{
		birdVelocity.x = birdVelocity.x * -1;
	}

	if (currentSpritePos.y > 725)
	{
		this->setActive(false);
	}

}

/*
=================================================================
  Sets the velocity for the Bird
=================================================================
*/
void cBird::setBirdVelocity(SDL_Point BirdVel)
{
	birdVelocity = BirdVel;
}

/*
=================================================================
  Gets the Bird velocity
=================================================================
*/
SDL_Point cBird::getBirdVelocity()
{
	return birdVelocity;
}
