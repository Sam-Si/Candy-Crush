#pragma once

#include <memory>
#include "SceneController.h"
#include "../controllers/ResourceController.h"

/*
	Root of the game.
	Creates controllers using RAII, initializes them and starts the game loop.
*/
class Engine
{
public:
	Engine();
	~Engine();

	// Deleted copy/move to prevent accidental copying
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;

	bool initGame();		// load resources, sets background and generate level
	void initControllers();	// inits controllers
	void startGame();		// starts game and game loop
	void destroyGame();		// destroy game

private:
	// Controllers owned via unique_ptr - automatic cleanup
	std::unique_ptr<SceneController> sceneController;
	std::unique_ptr<ResourceController> resourceController;
};

