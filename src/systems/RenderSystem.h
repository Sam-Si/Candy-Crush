#pragma once

#include "../game/GameState.h"
#include "../controllers/ResourceController.h"
#include <SDL2/SDL.h>

/*
	RenderSystem: Handles all rendering of game entities.
	Renders background, UI, and all entities with SpriteComponent.
*/
class RenderSystem
{
public:
	RenderSystem(GameState& state);
	~RenderSystem();

	// Render the entire frame
	void render();

private:
	GameState& gameState;

	// Render all entities with SpriteComponent
	void renderEntities();
};