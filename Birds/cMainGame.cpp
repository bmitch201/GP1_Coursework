/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cMainGame.h"


cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}

/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	theScore = 0;

	// Store the textures
	textureName = { "theBird", "thePlane","theBackground" };
	texturesToUse = { "Images\\Bird.png", "Images\\Plane2.png", "Images\\background.png" };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
		{
			theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
		}

	// Create textures for Game Dialogue (text)
	fontList = { "pressStart2P", "fipps" };
	fontsToUse = { "Fonts/PressStart2P.ttf", "Fonts/Fipps-Regular.otf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
		{
			theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
		}

	gameTextList = { "Birds", "Score:" };

	string scoreAsString = "Score:" + std::to_string(theScore);
	LPCSTR score = scoreAsString.c_str();
	gameTextList[1] = score;

	theTextureMgr->addTexture("Title", theFontMgr->getFont("fipps")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Score", theFontMgr->getFont("pressStart2P")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "explosion" };
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/Retro Beat.ogg", "Audio/explosion2.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
		{
			theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
		}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	thePlane.setSpritePos({ 500, 550 });
	thePlane.setSpriteRotAngle({ 90 });
	thePlane.setTexture(theTextureMgr->getTexture("thePlane"));
	thePlane.setSpriteDimensions(theTextureMgr->getTexture("thePlane")->getTWidth(), theTextureMgr->getTexture("thePlane")->getTHeight());
	thePlane.setPlaneVelocity({ 0, 0 });

	// Create vector array of textures
	for (int bird = 0; bird < 20; bird++)
		{
			theBirds.push_back(new cBird);
			theBirds[bird]->setSpritePos({ 90 * (rand() % 10), -125 * (rand() % 10) });
			theBirds[bird]->setSpriteTranslation({1, 5 });
			theBirds[bird]->setTexture(theTextureMgr->getTexture("theBird"));
			theBirds[bird]->setSpriteDimensions(theTextureMgr->getTexture(("theBird"))->getTWidth(), theTextureMgr->getTexture(("theBird"))->getTHeight());
			theBirds[bird]->setBirdVelocity({ 0, 25 });
			theBirds[bird]->setActive(true);
		}

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	bool loop = true;

	while (loop)
	{

		//The Planes x Boundaries
		if (thePlane.getSpritePos().x < 0)
		{
			thePlane.setSpritePos({ 0, thePlane.getSpritePos().y });
		}

		if (thePlane.getSpritePos().x > renderWidth - thePlane.getSpritePos().w)
		{
			thePlane.setSpritePos({ renderWidth - thePlane.getSpritePos().w, thePlane.getSpritePos().y });
		}

		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
	// Render each asteroid in the vector array
	for (int draw = 0; draw < theBirds.size(); draw++)
	{
		theBirds[draw]->render(theRenderer, &theBirds[draw]->getSpriteDimensions(), &theBirds[draw]->getSpritePos(), theBirds[draw]->getSpriteRotAngle(), &theBirds[draw]->getSpriteCentre(), theBirds[draw]->getSpriteScale());
	}
	
	// Render the Title
	cTexture* titleTextTexture = theTextureMgr->getTexture("Title");
	SDL_Rect posT = { 10, -10, titleTextTexture->getTextureRect().w, titleTextTexture->getTextureRect().h };
	FPoint scaleT = { 3, 3 };
	titleTextTexture->renderTexture(theRenderer, titleTextTexture->getTexture(), &titleTextTexture->getTextureRect(), &posT, scaleT);
	
	// Render the Score
	cTexture* scoreTextTexture = theTextureMgr->getTexture("Score");
	SDL_Rect posS = { 725, 15, scoreTextTexture->getTextureRect().w, scoreTextTexture->getTextureRect().h };
	FPoint scaleS = { 1, 1 };
	scoreTextTexture->renderTexture(theRenderer, scoreTextTexture->getTexture(), &scoreTextTexture->getTextureRect(), &posS, scaleS);
	
	// Render the Plane
	thePlane.render(theRenderer, &thePlane.getSpriteDimensions(), &thePlane.getSpritePos(), thePlane.getSpriteRotAngle(), &thePlane.getSpriteCentre(), thePlane.getSpriteScale());
	SDL_RenderPresent(theRenderer);

}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{
	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// Update the visibility and position of each bird
	vector<cBird*>::iterator birdIterator = theBirds.begin();
	while (birdIterator != theBirds.end())
	{
		if ((*birdIterator)->isActive() == false)
		{
			birdIterator = theBirds.erase(birdIterator);
		}
		else
		{
			(*birdIterator)->update(deltaTime);
			++birdIterator;
		}
	}

	for (birdIterator = theBirds.begin(); birdIterator != theBirds.end(); ++birdIterator)
	{
		if ((*birdIterator)->collidedWith(&(*birdIterator)->getBoundingRect(), &thePlane.getBoundingRect()))
		{
			thePlane.setActive(false);
			(*birdIterator)->setActive(false);
		}
	}

	// Update the Planes position
	thePlane.update(deltaTime);
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_LEFT:
					{
						thePlane.setSpriteTranslation({ 0, 0 });
					}
					break;

					case SDLK_RIGHT:
					{
						thePlane.setSpriteTranslation({ 0, 0 });
					}
					break;
				}
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						theLoop = false;
					break;

					case SDLK_LEFT:
					{
						if (thePlane.getSpritePos().x <= (renderWidth - thePlane.getSpritePos().w))
						{
							thePlane.setSpriteTranslation({ -10, -10 });
						}
					}
					break;

					case SDLK_RIGHT:
					{
						if (thePlane.getSpritePos().x >= 0)
						{
							thePlane.setSpriteTranslation({ 10, 10 });
						}
					}
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

