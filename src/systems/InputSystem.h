#pragma once

#include "../ecs/ISystem.h"
#include "../game/GameState.h"
#include "../game/MatchLogic.h"
#include "../common/Common.h"
#include <SDL2/SDL.h>
#include <vector>

/*
	InputSystem: Handles mouse input for entity selection and drag-and-drop.
	Processes SDL events and performs valid swaps between entities.
*/
class InputSystem : public ISystem
{
public:
	InputSystem(GameState& state);
	~InputSystem() override;

	// Queue an SDL event for processing during update
	void handleEvent(const SDL_Event& event);

	// Process all queued input events
	void update(float dt) override;

private:
	GameState& gameState;
	std::vector<SDL_Event> eventQueue;

	// Event handlers
	void onMouseButtonDown(const SDL_Event& event);
	void onMouseButtonUp(const SDL_Event& event);
	void onMouseMotion(const SDL_Event& event);

	// Helper methods
	EntityID pickEntity(const Point& point) const;
	void performMove(EntityID from, EntityID to);
	bool isValidSwap(EntityID from, EntityID to) const;
};