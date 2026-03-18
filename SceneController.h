#pragma once

#include "GameState.h"
#include "ISystem.h"
#include "InputSystem.h"
#include "MatchSystem.h"
#include "GravitySystem.h"
#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "UIManager.h"
#include "AudioSystem.h"
#include "ScoreSystem.h"
#include "EventBus.h"
#include <SDL2/SDL.h>
#include <memory>

/*
	SceneController: Pure orchestrator for game systems.
	Initializes all systems and calls them in order each frame.
	No game logic - only system coordination.
*/
class SceneController
{
public:
	SceneController();
	~SceneController();

	// Lifecycle
	bool init();
	void cleanup();

	// Per-frame operations
	void update();
	void renderScene();
	void handleEvent(SDL_Event* event);

	// Accessors
	SDL_Renderer* getRenderer();
	void setBackground(SDL_Texture* bg);
	void setGameStartTime(Uint32 time);

	// Game control
	void generateLevel();
	bool shouldQuit() const { return gameState.quit; }

private:
	// Shared game state
	GameState gameState;

	// SDL resources
	SDL_Window* gameWindow = nullptr;
	SDL_Surface* gameSurface = nullptr;

	// Systems
	std::unique_ptr<InputSystem> inputSystem;
	std::unique_ptr<MatchSystem> matchSystem;
	std::unique_ptr<GravitySystem> gravitySystem;
	std::unique_ptr<AudioSystem> audioSystem;
	std::unique_ptr<ScoreSystem> scoreSystem;
	std::unique_ptr<AnimationSystem> animationSystem;
	std::unique_ptr<RenderSystem> renderSystem;
	std::unique_ptr<UIManager> uiManager;

	// Entity manager and board
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<Board> board;

	// EventBus for decoupled system communication
	std::unique_ptr<EventBus> eventBus;

	// Internal helpers
	void updateInputState();
	EntityID createGamePiece(int x, int y, const std::string& color, bool draggable);
};
