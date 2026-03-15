#pragma once

#include "ECS.h"
#include "Board.h"
#include "AnimationSystem.h"
#include "UIManager.h"
#include "Common.h"
#include <set>
#include <queue>
#include <SDL2/SDL.h>

/*
	GameState: Shared state container for all systems.
	Holds game state, flags, and pointers to shared resources.
*/
struct GameState
{
	// Core ECS and board
	EntityManager* entityManager = nullptr;
	Board* board = nullptr;

	// Systems
	AnimationSystem* animationSystem = nullptr;
	UIManager* uiManager = nullptr;

	// Rendering
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* background = nullptr;

	// Input state
	EntityID selectedEntity = NULL_ENTITY;
	bool handleInput = true;

	// Game state
	bool gameOver = false;
	bool quit = false;
	int score = 0;
	Uint32 gameStartTime = 0;

	// Match/collapse state
	std::set<EntityID> comboItems;
	std::queue<SwapData> swaps;
	bool collapseFlag = false;
	bool moveFlag = false;
	bool fillFlag = false;

	// Helper methods
	bool isAnimating() const
	{
		return animationSystem && animationSystem->hasAnimations(*entityManager);
	}

	bool checkIfTimeIsUp() const
	{
		Uint32 current = SDL_GetTicks();
		Uint32 passed_seconds = (current - gameStartTime) / 1000;
		return passed_seconds > GAME_DURATION;
	}
};