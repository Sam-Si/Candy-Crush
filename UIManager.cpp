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

	if (gameoverTexture)
	{
		SDL_DestroyTexture(gameoverTexture);
		gameoverTexture = nullptr;
	}

	if (gFont)
	{
		TTF_CloseFont(gFont);
		gFont = nullptr;
	}

	renderer = nullptr;
}

bool UIManager::init(SDL_Renderer* renderer)
{
	this->renderer = renderer;
	bool success = true;

	// Open the font
	gFont = TTF_OpenFont("Aller_Rg.ttf", 28);
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

void UIManager::showGameOver()
{
	// Set up game over rect for rendering
	gameoverRect.x = (GAME_WITDH - GAMEOVER_WIDTH) / 2;
	gameoverRect.y = (GAME_HEIGHT - GAMEOVER_HEIGHT) / 2;
	gameoverRect.w = GAMEOVER_WIDTH;
	gameoverRect.h = GAMEOVER_HEIGHT;
	gameOverVisible = true;
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

	// Render game over if visible
	if (gameOverVisible)
	{
		SDL_Texture* goTexture = ResourceController::getResource(GAMEOVER);
		if (goTexture)
		{
			SDL_RenderCopyEx(renderer, goTexture, nullptr, &gameoverRect, 0.0, nullptr, SDL_FLIP_NONE);
		}
	}
}
