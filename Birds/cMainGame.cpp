/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cMainGame.h"


cMainGame* cMainGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cMainGame::cMainGame()
{

}

/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cMainGame* cMainGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cMainGame();
	}
	return cMainGame::pInstance;
}


void cMainGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	gamePlaying = false;
	
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 0);
	SDL_RenderPresent(theRenderer);

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	theScore = 0;

	//Store button textures
	btnNameList = { "exit_btn", "rules_btn", "menu_btn", "start_btn", "retry_btn", "resume_btn", "save_btn", "load_btn" };
	btnTexturesToUse = { "Images/Buttons/button_quit.png", "Images/Buttons/button_rules.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_start.png", "Images/Buttons/button_retry.png", "Images/Buttons/button_resume.png", "Images/Buttons/button_save.png", "Images/Buttons/button_load.png" };
	btnPos = { { 400, 375 },{ 400, 300 },{ 400, 300 },{ 500, 500 },{ 400, 300 },{ 740, 500 },{ 400, 300 },{ 400, 300 } };
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	
	theGameState = MENU;

	// Store sprite textures
	textureName = { "theBird", "thePlane","theBackground" };
	texturesToUse = { "Images\\Bird.png", "Images\\Plane2.png", "Images\\background.png" };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
		{
			theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
		}

	// Create textures for Game Dialogue (text)
	fontList = { "pressStart2P", "fipps", };
	fontsToUse = { "Fonts/PressStart2P.ttf", "Fonts/Fipps-Regular.otf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
		{
			theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
		}

	gameTextList = { "Birds", "Score:", "Game Over", " Move with the Left ", " and Right arrow keys", "Avoid the Birds to score" };

	scoreAsString = "Score:" + std::to_string(theScore);
	score = scoreAsString.c_str();
	gameTextList[1] = score;

	theTextureMgr->addTexture("Title", theFontMgr->getFont("fipps")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Score:", theFontMgr->getFont("pressStart2P")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("GameOver", theFontMgr->getFont("pressStart2P")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 0, 0, 0, 0 }, { 5, 5, 5, 0 }));
	theTextureMgr->addTexture("Instructions1", theFontMgr->getFont("pressStart2P")->createTextTexture(theRenderer, gameTextList[3], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Instructions2", theFontMgr->getFont("pressStart2P")->createTextTexture(theRenderer, gameTextList[4], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Instructions3", theFontMgr->getFont("pressStart2P")->createTextTexture(theRenderer, gameTextList[5], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "over", "explosion", "point" };
	soundTypes = { MUSIC, SFX, SFX, SFX };
	soundsToUse = { "Audio/Retro Beat.ogg", "Audio/Over.wav", "Audio/Explosion.wav", "Audio/Points.wav" };
	
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	//This does play
	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	thePlane.setSpritePos({ 500, 550 });
	thePlane.setSpriteRotAngle({ 90 });
	thePlane.setTexture(theTextureMgr->getTexture("thePlane"));
	thePlane.setSpriteDimensions(theTextureMgr->getTexture("thePlane")->getTWidth(), theTextureMgr->getTexture("thePlane")->getTHeight());
	thePlane.setPlaneVelocity({ 0, 0 });

	birdcount = 0;

	highScore = { 0,0,0,0,0,0 };

}

void cMainGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cMainGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	
	switch (theGameState)
	{
		case MENU:
		{
			spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		
			// Render the Title
			tempTextTexture = theTextureMgr->getTexture("Title");
			pos = { 450, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 1, 1 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
			// Render Buttons
			theButtonMgr->getBtn("start_btn")->setSpritePos({ 480, 300 });
			theButtonMgr->getBtn("start_btn")->render(theRenderer, &theButtonMgr->getBtn("start_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("start_btn")->getSpritePos(), theButtonMgr->getBtn("start_btn")->getSpriteScale());

			theButtonMgr->getBtn("rules_btn")->setSpritePos({ 480, 385 });
			theButtonMgr->getBtn("rules_btn")->render(theRenderer, &theButtonMgr->getBtn("rules_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("rules_btn")->getSpritePos(), theButtonMgr->getBtn("rules_btn")->getSpriteScale());
			
			theButtonMgr->getBtn("exit_btn")->setSpritePos({ 480, 470 });
			theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		}
		break;

	case RULES:
		{
			spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

			// Render the Title
			tempTextTexture = theTextureMgr->getTexture("Title");
			pos = { 450, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 1, 1 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

			// Render the Text
			tempTextTexture = theTextureMgr->getTexture("Instructions1");
			pos = { 100, 150, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 0.5, 0.5 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

			// Render the Text
			tempTextTexture = theTextureMgr->getTexture("Instructions2");
			pos = { 100, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 0.5, 0.5 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

			// Render the Text
			tempTextTexture = theTextureMgr->getTexture("Instructions3");
			pos = { 100, 325, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 0.5, 0.5 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

			// Render Button
			theButtonMgr->getBtn("menu_btn")->setSpritePos({ 480, 600 });
			theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
			
		}
		break;

	case PLAYING:
		{
		scoreUp = true;
		
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

			// Render each bird in the vector array
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
			cTexture* scoreTextTexture = theTextureMgr->getTexture("Score:");
			SDL_Rect posS = { 725, 15, scoreTextTexture->getTextureRect().w, scoreTextTexture->getTextureRect().h };
			FPoint scaleS = { 1, 1 };
			scoreTextTexture->renderTexture(theRenderer, scoreTextTexture->getTexture(), &scoreTextTexture->getTextureRect(), &posS, scaleS);
			
			//Render the score when changed
			if (scoreUp = true)
			{
				if (theTextureMgr->getTexture("Score:") != NULL)
				{
					theTextureMgr->deleteTexture("Score:");
					theTextureMgr->addTexture("Score:", theFontMgr->getFont("pressStart2P")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));
					scoreUp = false;
				}

			}

			// Render the Plane
			thePlane.render(theRenderer, &thePlane.getSpriteDimensions(), &thePlane.getSpritePos(), thePlane.getSpriteRotAngle(), &thePlane.getSpriteCentre(), thePlane.getSpriteScale());

		}
	break;

	case GAMEOVER:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("GameOver");
		pos = { 400, 50, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 2, 2 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		tempTextTexture = theTextureMgr->getTexture("Score:");
		pos = { 400, 300, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theButtonMgr->getBtn("retry_btn")->setSpritePos({ 480, 615 });
		theButtonMgr->getBtn("retry_btn")->render(theRenderer, &theButtonMgr->getBtn("retry_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("retry_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());

		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 480, 700 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;

	case END:
	{	
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("Title");
		pos = { 480, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render Button
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 480, 300 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		
		theButtonMgr->getBtn("resume_btn")->setSpritePos({ 480, 385 });
		theButtonMgr->getBtn("resume_btn")->render(theRenderer, &theButtonMgr->getBtn("resume_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("resume_btn")->getSpritePos(), theButtonMgr->getBtn("resume_btn")->getSpriteScale());

		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 480, 470 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	
	case QUIT:
	{
		loop = false;
	}
	break;
	default:
		break;
	}

	SDL_RenderPresent(theRenderer);
}

void cMainGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{
	SDL_RenderPresent(theRenderer);
}

void cMainGame::update()
{
}

void cMainGame::update(double deltaTime)
{
	// Check Button clicked and change state
	switch (theGameState)
	{
	
	case MENU:
		{
			theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
			theGameState = theButtonMgr->getBtn("rules_btn")->update(theGameState, RULES, theAreaClicked);
			theGameState = theButtonMgr->getBtn("start_btn")->update(theGameState, PLAYING, theAreaClicked);
			theAreaClicked = { 0, 0 };
		}
	break;

	case RULES:
	{
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
		theAreaClicked = { 0, 0 };
	}
	break;

	case PLAYING:
	{
		gamePlaying = true;

		if (birdcount < 6)
		{
			theBirds.push_back(new cBird);
			int bird = theBirds.size() - 1;
			theBirds[bird]->setTexture(theTextureMgr->getTexture("theBird"));
			theBirds[bird]->setSpriteDimensions(theTextureMgr->getTexture(("theBird"))->getTWidth(), theTextureMgr->getTexture(("theBird"))->getTHeight());
			theBirds[bird]->setSpritePos({ 90 * (rand() % 10), -125 * (rand() % 10) });
			theBirds[bird]->setSpriteTranslation({ 1, 5 });
			theBirds[bird]->setActive(true);
			birdcount++;
		}


		//Add Speed to the birds. Delete the birds if they go off screen. Adds the score if players successfully dodge the birds
		for (int bird = 0; bird < theBirds.size(); bird++)
		{
			theBirds[bird]->setBirdVelocity({ 0, 25 });
			if (theBirds[bird]->getSpritePos().y > 700)
			{
				theBirds[bird]->setActive(false);
				birdcount--;
				theScore++;
				theSoundMgr->getSnd("point")->play(0);
				scoreAsString = "Score:" + std::to_string(theScore);
				score = scoreAsString.c_str();
				gameTextList[1] = score;
				scoreUp = true;
			}
		}

		//The Planes x Boundaries
		if (thePlane.getSpritePos().x < 0)
		{
			thePlane.setSpritePos({ 0, thePlane.getSpritePos().y });
		}

		if (thePlane.getSpritePos().x > renderWidth - thePlane.getSpritePos().w)
		{
			thePlane.setSpritePos({ renderWidth - thePlane.getSpritePos().w, thePlane.getSpritePos().y });
		}

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

			if (!gamePlaying)
			{
				for (int bird = 0; bird < theBirds.size(); bird++)
				{
					theBirds[bird]->setBirdVelocity({ 0, 0 });
				}
			}
		}

		//Check if the Plane collides with any birds
		for (birdIterator = theBirds.begin(); birdIterator != theBirds.end(); ++birdIterator)
		{
			if ((*birdIterator)->collidedWith(&(*birdIterator)->getBoundingRect(), &thePlane.getBoundingRect()))
			{
				(*birdIterator)->setActive(false);
				theSoundMgr->getSnd("explosion")->play(0);
				thePlane.setSpritePos({ 500, 900 });

				for (int count = 4; count >= 0; count--)
				{
					if (theScore > highScore[count])
					{
						highScore[count] = theScore;
						break;
					}
					
				}

				cout << highScore[0];
				cout << highScore[1];
				cout << highScore[2];
				cout << highScore[3];
				cout << highScore[4];

				theGameState = GAMEOVER;
				theSoundMgr->getSnd("over")->play(3);
			}
		}

		// Update the Planes position
		thePlane.update(deltaTime);
	}
	break;

	case END:
	{
		gamePlaying = false;
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, RESTART, theAreaClicked);
		theGameState = theButtonMgr->getBtn("resume_btn")->update(theGameState, PLAYING, theAreaClicked);
		theAreaClicked = { 0, 0 };

	}
		break;

	case GAMEOVER:
	{
		gamePlaying = false;
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = theButtonMgr->getBtn("retry_btn")->update(theGameState, RETRY, theAreaClicked);
		theAreaClicked = { 0, 0 };

	}
		break;

	case RETRY:
	{
		thePlane.setSpritePos({ 500, 550 });
		theBirds.clear();
		birdcount = 0;
		theScore = 0;
		scoreAsString = "Score:" + std::to_string(theScore);
		score = scoreAsString.c_str();
		gameTextList[1] = score;
		theGameState = PLAYING;
	}
	break;

	case RESTART:
	{
		thePlane.setSpritePos({ 500, 550 });
		theBirds.clear();
		birdcount = 0;
		theScore = 0;
		scoreAsString = "Score:" + std::to_string(theScore);
		score = scoreAsString.c_str();
		gameTextList[1] = score;
		theGameState = MENU;
	}
	break;

	case QUIT:
		{
		}
		break;
		default:
	break;
	}
}

bool cMainGame::getInput(bool theLoop)
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

		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
				theAreaClicked = { event.motion.x, event.motion.y };
			}
			break;
			default:
				break;
			}

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
			{
				switch (theGameState)
				{

				case MENU:
				{
					theLoop = false;
				}
				break;

				case PLAYING:
				{
					theGameState = END;

					for (int count = 4; count >= 0; count--)
					{
						if (theScore > highScore[count])
						{
							highScore[count] = theScore;
							break;
						}
					}
					cout << highScore[0];
					cout << highScore[1];
					cout << highScore[2];
					cout << highScore[3];
					cout << highScore[4];
				}
				break;

				case GAMEOVER:
				{
					theLoop = false;
				}
				break;

				case END:
				{
					theLoop = false;
				}
				break;

				}
				break;
			}


			case SDLK_LEFT:
			{

				if (gamePlaying && thePlane.getSpritePos().x <= (renderWidth - thePlane.getSpritePos().w))
				{
					thePlane.setSpriteTranslation({ -10, -10 });
				}
			}
			break;

			case SDLK_RIGHT:
			{
				if (gamePlaying && thePlane.getSpritePos().x >= 0)
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

double cMainGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cMainGame::cleanUp(SDL_Window* theSDLWND)
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

