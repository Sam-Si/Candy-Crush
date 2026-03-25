#include "RenderSystem.h"
#include "../ecs/Components.h"

RenderSystem::RenderSystem(GameState& state)
	: gameState(state)
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::render()
{
	if (!gameState.renderer)
	{
		return;
	}

	// Clear screen
	SDL_SetRenderDrawColor(gameState.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gameState.renderer);

	// Render background
	if (gameState.background)
	{
		SDL_RenderCopyEx(gameState.renderer, gameState.background, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
	}

	// Render UI
	if (gameState.uiManager)
	{
		gameState.uiManager->render();
	}

	// Render all game entities
	renderEntities();

	// Present frame
	SDL_RenderPresent(gameState.renderer);
}

void RenderSystem::renderEntities()
{
	const auto& sprites = gameState.entityManager->getAllComponents<SpriteComponent>();

	for (const auto& pair : sprites)
	{
		const SpriteComponent& sprite = pair.second;

		SDL_Texture* texture = ResourceController::getResource(sprite.textureId);
		if (texture)
		{
			// Convert generic rect to SDL_Rect for rendering
			SDL_Rect destRect;
			destRect.x = static_cast<int>(sprite.x);
			destRect.y = static_cast<int>(sprite.y);
			destRect.w = static_cast<int>(sprite.width);
			destRect.h = static_cast<int>(sprite.height);
			SDL_RenderCopy(gameState.renderer, texture, NULL, &destRect);
		}
	}
}