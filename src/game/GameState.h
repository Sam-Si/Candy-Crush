#pragma once

#include "../ecs/ECS.h"
#include "Board.h"
#include "../systems/AnimationSystem.h"
#include "../controllers/UIManager.h"
#include "../events/EventBus.h"
#include "../common/Common.h"
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

	// EventBus for decoupled system communication (owned by SceneController)
	EventBus* eventBus = nullptr;

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

	/*
		GravityPhase: Self-documenting state machine for the post-match gravity cycle.
		Flow: IDLE → AWAITING_COLLAPSE → COLLAPSING → AWAITING_FILL → IDLE

		- IDLE:           Normal gameplay, no gravity operations pending.
		- AWAITING_COLLAPSE: Matches found, entities marked for destruction.
		                   Wait for destroy animations to complete.
		- COLLAPSING:     Destroy animations done. Pieces are falling (move animations).
		                   Wait for fall animations to complete.
		- AWAITING_FILL:  All falling complete. Need to spawn new candies at empty slots.
	*/
	enum class GravityPhase
	{
		IDLE,
		AWAITING_COLLAPSE,
		COLLAPSING,
		AWAITING_FILL
	};
	GravityPhase gravityPhase = GravityPhase::IDLE;

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

	// Returns true if a gravity cycle is in progress (not idle).
	// Used by MatchSystem to pause match detection during gravity operations.
	bool isGravityCycleActive() const
	{
		return gravityPhase != GravityPhase::IDLE;
	}
};