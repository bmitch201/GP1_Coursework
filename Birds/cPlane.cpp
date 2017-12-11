/*
=================
cRocket.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cPlane.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cPlane::cPlane() : cSprite()
{
	this->planeVelocity = { 0, 0 };
}
/*
=================================================================
Update the plane position
=================================================================
*/

void cPlane::update(double deltaTime)
{

	FPoint direction = { 0.0f, 0.0f };
	direction.X = (sin(this->getSpriteRotAngle()*PI / 180));
	direction.Y = -(cos(this->getSpriteRotAngle()*PI / 180));

	direction.X *= this->getSpriteTranslation().x;
	direction.Y *= this->getSpriteTranslation().y;

	this->planeVelocity.x = this->planeVelocity.x + direction.X;
	this->planeVelocity.y = this->planeVelocity.y + direction.Y;

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += this->planeVelocity.x * deltaTime;
	currentSpritePos.y += this->planeVelocity.y * deltaTime;

	this->setBoundingRect(this->getSpritePos());

	this->planeVelocity.x *= 0.95;
	this->planeVelocity.y *= 0.95;
	
	this->setSpritePos({ currentSpritePos.x , currentSpritePos.y  });
	this->setBoundingRect(this->getSpritePos());
}
/*
=================================================================
Sets the velocity for the plane
=================================================================
*/
void cPlane::setPlaneVelocity(SDL_Point planeVel)
{
	planeVelocity = planeVel;
}
/*
=================================================================
Gets the plane velocity
=================================================================
*/
SDL_Point cPlane::getPlaneVelocity()
{
	return planeVelocity;
}