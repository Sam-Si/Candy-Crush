#include "SceneController.h"
#include "../ecs/Components.h"
#include <SDL2/SDL_image.h>
#include <iostream>

SceneController::SceneController()
{
	// Create core ECS
	entityManager = std::make_unique<EntityManager>();
	board = std::make_unique<Board>();
	
	// Initialize game state
	gameState.entityManager = entityManager.get();
	gameState.board = board.get();
}

SceneController::~SceneController()
{
}

bool SceneController::init()
{
	bool success = true;
	
	board->initialize();

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "Error on init SDL" << std::endl;
		success = false;
	}
	else
	{
		gameWindow = SDL_CreateWindow("KingCrash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		                              GAME_WITDH, GAME_HEIGHT, SDL_WINDOW_SHOWN);
		if (gameWindow == nullptr)
		{
			std::cout << "Game window could not be created" << std::endl;
			success = false;
		}
		else
		{
			Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
#ifdef SOFTWARE_ACC
			rendererFlags = SDL_RENDERER_SOFTWARE;
#endif
			gameState.renderer = SDL_CreateRenderer(gameWindow, -1, rendererFlags);
			if (gameState.renderer == nullptr)
			{
				std::cout << "cannot create renderer" << std::endl;
				success = false;
			}
			else
			{
				int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				else
				{
					gameSurface = SDL_GetWindowSurface(gameWindow);
					if (TTF_Init() == -1)
					{
						printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
						success = false;
					}
				}
			}
		}
	}

	// Create EventBus for decoupled system communication
	eventBus = std::make_unique<EventBus>();
	gameState.eventBus = eventBus.get();

	// Create systems
	// Note: AudioSystem and ScoreSystem must be created before other systems
	// to ensure they subscribe to events before any events are published
	audioSystem = std::make_unique<AudioSystem>(gameState);
	scoreSystem = std::make_unique<ScoreSystem>(gameState);
	
	inputSystem = std::make_unique<InputSystem>(gameState);
	matchSystem = std::make_unique<MatchSystem>(gameState);
	gravitySystem = std::make_unique<GravitySystem>(gameState);
	animationSystem = std::make_unique<AnimationSystem>();
	renderSystem = std::make_unique<RenderSystem>(gameState);
	uiManager = std::make_unique<UIManager>();
	
	// Set up game state references
	gameState.animationSystem = animationSystem.get();
	gameState.uiManager = uiManager.get();
	
	bool uiResult = uiManager->init(gameState.renderer);
	
	return success && uiResult;
}

void SceneController::cleanup()
{
	if (board)
	{
		board->forEachEntity([this](EntityID entity) {
			entityManager->destroyEntity(entity);
		});
		board->clear();
	}

	if (uiManager)
	{
		uiManager->cleanup();
	}

	if (gameState.renderer)
	{
		SDL_DestroyRenderer(gameState.renderer);
	}
	if (gameWindow)
	{
		SDL_DestroyWindow(gameWindow);
	}
	
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void SceneController::update()
{
	// Update handleInput based on animation state
	updateInputState();

	// Check game over
	if (gameState.checkIfTimeIsUp())
	{
		if (!gameState.gameOver)
		{
			uiManager->showGameOver(gameState.score);
		}
		gameState.gameOver = true;
		return;
	}

	// Update UI
	uiManager->updateTime(gameState.gameStartTime);

	// Update systems in strict order:
	// 1. Input
	// 2. Logic (Matches, Gravity)
	// 3. Dispatch events
	// 4. Audio and Score systems (process dispatched events)
	// 5. Animation
	inputSystem->update(MS_60_MS);
	matchSystem->update(MS_60_MS);
	gravitySystem->update(MS_60_MS);

	// Dispatch events published by logic systems
	if (eventBus)
	{
		eventBus->dispatch();
	}

	// Process event-driven systems (Audio, Score)
	audioSystem->update(MS_60_MS);
	scoreSystem->update(MS_60_MS);

	// Process animations
	animationSystem->update(*entityManager, MS_60_MS);
}

void SceneController::renderScene()
{
	renderSystem->render();
}

void SceneController::handleEvent(SDL_Event* event)
{
	if (event->type == SDL_QUIT)
	{
		gameState.quit = true;
		return;
	}
	
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_CLOSE)
	{
		gameState.quit = true;
		return;
	}
	
	inputSystem->handleEvent(*event);
}

SDL_Renderer* SceneController::getRenderer()
{
	return gameState.renderer;
}

void SceneController::setBackground(SDL_Texture* bg)
{
	gameState.background = bg;
}

void SceneController::setGameStartTime(Uint32 time)
{
	gameState.gameStartTime = time;
}

void SceneController::updateInputState()
{
	if (animationSystem->hasAnimations(*entityManager))
	{
		gameState.handleInput = false;
	}
	else
	{
		gameState.handleInput = true;
	}
}

EntityID SceneController::createGamePiece(int x, int y, const std::string& color, bool draggable)
{
	EntityID entity = entityManager->createEntity();
	
	PositionComponent pos;
	pos.x = x;
	pos.y = y;
	entityManager->addComponent(entity, pos);
	
	SpriteComponent sprite;
	sprite.textureId = color;
	Rect r = calculateRenderRectFromCoordinate(x, y);
	sprite.x = r.x;
	sprite.y = r.y;
	sprite.width = r.width;
	sprite.height = r.height;
	entityManager->addComponent(entity, sprite);
	
	MatchableComponent matchable;
	matchable.colorId = color;
	entityManager->addComponent(entity, matchable);
	
	DraggableComponent drag;
	drag.isDraggable = draggable;
	entityManager->addComponent(entity, drag);
	
	return entity;
}

void SceneController::generateLevel()
{
	// Clear existing board and entities
	board->forEachEntity([this](EntityID entity) {
		entityManager->destroyEntity(entity);
	});
	board->clear();
	
	// Reset game state
	gameState.score = 0;
	gameState.comboItems.clear();
	while (!gameState.swaps.empty()) gameState.swaps.pop();
	gameState.gravityPhase = GameState::GravityPhase::IDLE;
	gameState.gameOver = false;
	gameState.handleInput = true;
	
	if (uiManager)
	{
		uiManager->updateScore(0);
		uiManager->hideGameOver();
	}
	
	// Generate new level
	std::string obj;
	for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
	{
		for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
		{
			do{
				obj = getRandomColorId();
			} while ((i >= 2 && 
			          entityManager->getComponent<MatchableComponent>(board->getEntity(i - 1, y))->colorId == obj && 
			          entityManager->getComponent<MatchableComponent>(board->getEntity(i - 2, y))->colorId == obj)
			    || (y >= 2 && 
			        entityManager->getComponent<MatchableComponent>(board->getEntity(i, y - 1))->colorId == obj && 
			        entityManager->getComponent<MatchableComponent>(board->getEntity(i, y - 2))->colorId == obj));

			EntityID entity = createGamePiece(i, y, obj, true);
			board->setEntity(i, y, entity);
		}
	}
}