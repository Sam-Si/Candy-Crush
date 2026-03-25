#pragma once

#include "../common/Common.h"
#include "ResourceController.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

/*
	UIManager handles all HUD and text rendering.
	Encapsulates font management, score/time display, and game over screen.
	SceneController maintains game state (score, time) and delegates rendering.
	Uses SDL_Rect and textures directly instead of DrawableEntity.
*/
class UIManager
{
public:
	UIManager();
	~UIManager();

	// Initialize fonts and text rendering
	bool init(SDL_Renderer* renderer);

	// Update score display with current score value
	void updateScore(int score);

	// Update time display based on game start time
	void updateTime(Uint32 gameStartTime);

	// Show game over screen with final score
	void showGameOver(int finalScore);

	// Hide game over screen
	void hideGameOver();

	// Check if game over is currently displayed
	bool isGameOverVisible() const;

	// Render all HUD elements
	void render();

	// Cleanup resources
	void cleanup();

private:
	SDL_Renderer* renderer = nullptr;

	// Font resources
	TTF_Font* gFont = nullptr;

	// Score display
	SDL_Surface* scoreSurface = nullptr;
	SDL_Texture* scoreTexture = nullptr;
	SDL_Rect scoreTextRect;

	// Time display
	SDL_Surface* timeSurface = nullptr;
	SDL_Texture* timeTexture = nullptr;
	SDL_Rect timeTextRect;

	// Game over state (pure score display, no image)
	bool gameOverVisible = false;
	int finalScore = 0;

	// Game over text display
	SDL_Surface* gameOverSurface = nullptr;
	SDL_Texture* gameOverTexture = nullptr;
	SDL_Rect gameOverTextRect;

	// Helper methods
	bool createScoreTexture(const std::string& text, SDL_Color color);
	bool createTimeTexture(const std::string& text, SDL_Color color);
	void freeScoreResources();
	void freeTimeResources();
	void freeGameOverResources();
};
