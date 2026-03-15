#include "Engine.h"
#include <SDL2/SDL.h>
#include <iostream>

Engine::Engine() = default;

Engine::~Engine() = default;

void Engine::destroyGame()
{
}

void Engine::initControllers()
{
	sceneController = std::make_unique<SceneController>();
	resourceController = std::make_unique<ResourceController>();
	sceneController->init();
	resourceController->init(sceneController->getRenderer());
}

bool Engine::initGame()
{
	srand(time(NULL));
	
	bool success = true;
	if (resourceController->loadResources() == false)
	{
		success = false;
	}
	
	SDL_Texture* bg = resourceController->getResource("background");
	sceneController->setBackground(bg);
	sceneController->generateLevel();
	
	return success;
}

void Engine::startGame()
{
	bool quit = false;
	double previous = SDL_GetTicks();
	double lag = 0.0f;
	sceneController->setGameStartTime(previous);
	
	while (!quit)
	{		
		if (sceneController->shouldQuit())
		{
			quit = true;
		}
		
		double current = SDL_GetTicks();
		double elapsed = current - previous;
		previous = current;
		lag += elapsed;
		
		while (lag >= MS_60_MS)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				sceneController->handleEvent(&event);
			}
			sceneController->update();
			lag -= MS_60_MS;
		}
		
		sceneController->renderScene();
	}

	// RAII cleanup: sceneController and resourceController 
	// are automatically destroyed when Engine is destroyed
	sceneController->cleanup();
}
