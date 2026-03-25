#include "UIManager.h"
#include <sstream>

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
	cleanup();
}

void UIManager::cleanup()
{
	freeScoreResources();
	freeTimeResources();
	freeGameOverResources();

	if (gFont)
	{
		TTF_CloseFont(gFont);
		gFont = nullptr;
	}

	renderer = nullptr;
}

void UIManager::freeGameOverResources()
{
	if (gameOverTexture)
	{
		SDL_DestroyTexture(gameOverTexture);
		gameOverTexture = nullptr;
	}
	if (gameOverSurface)
	{
		SDL_FreeSurface(gameOverSurface);
		gameOverSurface = nullptr;
	}
}

bool UIManager::init(SDL_Renderer* renderer)
{
	this->renderer = renderer;
	bool success = true;

	// Open the font
	gFont = TTF_OpenFont("assets/fonts/Aller_Rg.ttf", 28);
	if (gFont == nullptr)
	{
		printf("Failed to load font! Continuing without text. SDL_ttf Error: %s\n", TTF_GetError());
		success = true;
	}
	else
	{
		updateScore(0);
		updateTime(0);
		success = true;
	}

	return success;
}

void UIManager::freeScoreResources()
{
	if (scoreTexture)
	{
		SDL_DestroyTexture(scoreTexture);
		scoreTexture = nullptr;
	}
	if (scoreSurface)
	{
		SDL_FreeSurface(scoreSurface);
		scoreSurface = nullptr;
	}
}

void UIManager::freeTimeResources()
{
	if (timeTexture)
	{
		SDL_DestroyTexture(timeTexture);
		timeTexture = nullptr;
	}
	if (timeSurface)
	{
		SDL_FreeSurface(timeSurface);
		timeSurface = nullptr;
	}
}

bool UIManager::createScoreTexture(const std::string& text, SDL_Color color)
{
	freeScoreResources();

	scoreSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
	if (scoreSurface == nullptr)
	{
		printf("Unable to render score text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
	if (scoreTexture == nullptr)
	{
		printf("Unable to create score texture from rendered text! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	scoreTextRect.w = scoreSurface->w;
	scoreTextRect.h = scoreSurface->h;

	SDL_FreeSurface(scoreSurface);
	scoreSurface = nullptr;

	return true;
}

bool UIManager::createTimeTexture(const std::string& text, SDL_Color color)
{
	freeTimeResources();

	timeSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
	if (timeSurface == nullptr)
	{
		printf("Unable to render time text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
	if (timeTexture == nullptr)
	{
		printf("Unable to create time texture from rendered text! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	timeTextRect.w = timeSurface->w;
	timeTextRect.h = timeSurface->h;
	timeTextRect.x = GAME_WITDH - timeSurface->w;
	timeTextRect.y = 0;

	SDL_FreeSurface(timeSurface);
	timeSurface = nullptr;

	return true;
}

void UIManager::updateScore(int score)
{
	if (gFont == nullptr) return;

	std::ostringstream scr;
	scr << score;
	std::string scoreStr = "Score:" + scr.str();

	SDL_Color textColor = { 0, 0, 0 };
	createScoreTexture(scoreStr, textColor);
}

void UIManager::updateTime(Uint32 gameStartTime)
{
	if (gFont == nullptr) return;

	Uint32 passed = (SDL_GetTicks() - gameStartTime) / 1000;
	Uint32 elapsed = GAME_DURATION - passed;

	std::ostringstream t;
	t << elapsed;

	std::string prefix;
	SDL_Color color = { 0, 0, 0 };

	if (elapsed < 10)
	{
		prefix = "00:0";
		color = { 255, 0, 0 };
	}
	else
	{
		prefix = "00:";
	}

	std::string timeStr = "Time Left:" + prefix + t.str();
	createTimeTexture(timeStr, color);
}

void UIManager::showGameOver(int score)
{
	finalScore = score;
	gameOverVisible = true;

	if (gFont == nullptr) return;

	freeGameOverResources();

	// Create game over text with final score
	std::ostringstream text;
	text << "Game Over! Final Score: " << finalScore;
	std::string gameOverStr = text.str();

	SDL_Color color = { 255, 0, 0 }; // Red text
	gameOverSurface = TTF_RenderText_Solid(gFont, gameOverStr.c_str(), color);
	if (gameOverSurface == nullptr)
	{
		return;
	}

	gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
	if (gameOverTexture == nullptr)
	{
		SDL_FreeSurface(gameOverSurface);
		gameOverSurface = nullptr;
		return;
	}

	// Center the text on screen
	gameOverTextRect.w = gameOverSurface->w;
	gameOverTextRect.h = gameOverSurface->h;
	gameOverTextRect.x = (GAME_WITDH - gameOverTextRect.w) / 2;
	gameOverTextRect.y = (GAME_HEIGHT - gameOverTextRect.h) / 2;

	SDL_FreeSurface(gameOverSurface);
	gameOverSurface = nullptr;
}

void UIManager::hideGameOver()
{
	gameOverVisible = false;
}

bool UIManager::isGameOverVisible() const
{
	return gameOverVisible;
}

void UIManager::render()
{
	if (renderer == nullptr) return;

	// Render score
	if (scoreTexture)
	{
		SDL_RenderCopyEx(renderer, scoreTexture, nullptr, &scoreTextRect, 0.0, nullptr, SDL_FLIP_NONE);
	}

	// Render time
	if (timeTexture)
	{
		SDL_RenderCopyEx(renderer, timeTexture, nullptr, &timeTextRect, 0.0, nullptr, SDL_FLIP_NONE);
	}

	// Render game over if visible (text-based, no image)
	if (gameOverVisible && gameOverTexture)
	{
		SDL_RenderCopyEx(renderer, gameOverTexture, nullptr, &gameOverTextRect, 0.0, nullptr, SDL_FLIP_NONE);
	}
}
