#include "InputSystem.h"
#include "../game/MatchLogic.h"
#include "../events/Events.h"
#include <cmath>
#include <iostream>

InputSystem::InputSystem(GameState& state)
	: gameState(state)
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::handleEvent(const SDL_Event& event)
{
	eventQueue.push_back(event);
}

void InputSystem::update(float dt)
{
	// Handle game over state first (keyboard events for restart/quit)
	for (const auto& event : eventQueue)
	{
		if (event.type == SDL_KEYDOWN && gameState.gameOver)
		{
			if (event.key.keysym.sym == SDLK_RETURN)
			{
				// Signal to regenerate level - SceneController handles this
				gameState.gameOver = false;
				gameState.handleInput = true;
				gameState.gameStartTime = SDL_GetTicks();
				// Clear all entities and regenerate
				gameState.board->forEachEntity([&](EntityID entity) {
					gameState.entityManager->destroyEntity(entity);
				});
				gameState.board->clear();
				// Note: generateLevel will be called by SceneController
			}
			else if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				gameState.quit = true;
			}
		}
	}

	// Skip input processing if game over or animations playing
	if (gameState.gameOver || !gameState.handleInput || gameState.isAnimating())
	{
		eventQueue.clear();
		return;
	}

	// Process input events
	for (const auto& event : eventQueue)
	{
		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				onMouseButtonDown(event);
				break;
			case SDL_MOUSEBUTTONUP:
				onMouseButtonUp(event);
				break;
			case SDL_MOUSEMOTION:
				onMouseMotion(event);
				break;
			default:
				break;
		}
	}

	eventQueue.clear();
}

void InputSystem::onMouseButtonDown(const SDL_Event& event)
{
	Point p(event.button.x, event.button.y);
	gameState.selectedEntity = pickEntity(p);
}

void InputSystem::onMouseButtonUp(const SDL_Event& event)
{
	if (gameState.selectedEntity == NULL_ENTITY)
	{
		return;
	}

	Point t(event.button.x, event.button.y);

	EntityID targetEntity = pickEntity(t);

	// Validate the swap
	if (!isValidSwap(gameState.selectedEntity, targetEntity))
	{
		// Reset position visually
		PositionComponent* selectedPos = gameState.entityManager->getComponent<PositionComponent>(gameState.selectedEntity);
		SpriteComponent* sprite = gameState.entityManager->getComponent<SpriteComponent>(gameState.selectedEntity);
		if (sprite && selectedPos)
		{
			Rect r = calculateRenderRectFromCoordinate(selectedPos->x, selectedPos->y);
			sprite->x = r.x;
			sprite->y = r.y;
		}
		gameState.selectedEntity = NULL_ENTITY;
		return;
	}

	// Perform the move
	performMove(gameState.selectedEntity, targetEntity);
	gameState.selectedEntity = NULL_ENTITY;
}

void InputSystem::onMouseMotion(const SDL_Event& event)
{
	if ((event.motion.state & SDL_BUTTON_LMASK) && gameState.selectedEntity != NULL_ENTITY)
	{
		SpriteComponent* sprite = gameState.entityManager->getComponent<SpriteComponent>(gameState.selectedEntity);
		if (sprite)
		{
			sprite->x += static_cast<float>(event.motion.xrel);
			sprite->y += static_cast<float>(event.motion.yrel);
		}
	}
}

EntityID InputSystem::pickEntity(const Point& point) const
{
	Point coordinate;
	calculateCoordinateFromMousePos(point.x, point.y, coordinate);

	if (coordinate.x < 0 || coordinate.y < 0)
	{
		return NULL_ENTITY;
	}

	EntityID ent = gameState.board->getEntity(coordinate.x, coordinate.y);
	if (ent != NULL_ENTITY)
	{
		DraggableComponent* drag = gameState.entityManager->getComponent<DraggableComponent>(ent);
		if (drag && drag->isDraggable)
		{
			return ent;
		}
	}
	return NULL_ENTITY;
}

bool InputSystem::isValidSwap(EntityID from, EntityID to) const
{
	if (to == NULL_ENTITY || to == from)
	{
		return false;
	}

	PositionComponent* fromPos = gameState.entityManager->getComponent<PositionComponent>(from);
	PositionComponent* toPos = gameState.entityManager->getComponent<PositionComponent>(to);

	if (!fromPos || !toPos)
	{
		return false;
	}

	// Check if adjacent
	if (std::abs(toPos->x - fromPos->x) > 1 || std::abs(toPos->y - fromPos->y) > 1)
	{
		return false;
	}

	// Check if valid move (creates a match)
	return MatchLogic::checkIfValidMove(*gameState.board, *gameState.entityManager, from, to);
}

void InputSystem::performMove(EntityID from, EntityID to)
{
	// Publish SwapExecutedEvent for decoupled handling
	if (gameState.eventBus)
	{
		gameState.eventBus->publish(SwapExecutedEvent(from, to));
	}

	PositionComponent* fromPos = gameState.entityManager->getComponent<PositionComponent>(from);
	PositionComponent* toPos = gameState.entityManager->getComponent<PositionComponent>(to);

	if (!fromPos || !toPos)
	{
		return;
	}

	// Swap grid positions
	int tempx = fromPos->x;
	fromPos->x = toPos->x;
	toPos->x = tempx;

	int tempy = fromPos->y;
	fromPos->y = toPos->y;
	toPos->y = tempy;

	// Update board
	gameState.board->setEntity(fromPos->x, fromPos->y, from);
	gameState.board->setEntity(toPos->x, toPos->y, to);

	// Update sprite positions immediately
	SpriteComponent* fromSprite = gameState.entityManager->getComponent<SpriteComponent>(from);
	SpriteComponent* toSprite = gameState.entityManager->getComponent<SpriteComponent>(to);

	if (fromSprite)
	{
		Rect r = calculateRenderRectFromCoordinate(fromPos->x, fromPos->y);
		fromSprite->x = r.x;
		fromSprite->y = r.y;
	}
	if (toSprite)
	{
		Rect r = calculateRenderRectFromCoordinate(toPos->x, toPos->y);
		toSprite->x = r.x;
		toSprite->y = r.y;
	}
}