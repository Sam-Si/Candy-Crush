#include "GravitySystem.h"
#include "../game/GravityLogic.h"

GravitySystem::GravitySystem(GameState& state)
	: gameState(state)
{
}

GravitySystem::~GravitySystem()
{
}

/*
	GravitySystem::update

	The gravity cycle processes post-match board changes in phases.
	Each phase waits for animations to complete before advancing.

	Phase flow:
		IDLE -> AWAITING_COLLAPSE -> COLLAPSING -> AWAITING_FILL -> IDLE

	- IDLE: Nothing to do. MatchSystem may trigger AWAITING_COLLAPSE.
	- AWAITING_COLLAPSE: Destroy matched entities, then start falling.
	- COLLAPSING: Finalize falling positions, then fill empty slots.
	- AWAITING_FILL: No moves needed; just spawn new candies.
*/
void GravitySystem::update(float dt)
{
	// Gravity operations only proceed when no animations are playing.
	// Animations (destroy, move) visually represent the state transitions.
	if (gameState.isAnimating())
	{
		return;
	}

	switch (gameState.gravityPhase)
	{
		case GameState::GravityPhase::IDLE:
			// Nothing to do. MatchSystem sets AWAITING_COLLAPSE when matches are found.
			break;

		case GameState::GravityPhase::AWAITING_COLLAPSE:
			// Destroy animations completed. Time to remove matched entities and collapse.
			destroyMatchedEntities();
			startCollapse();
			break;

		case GameState::GravityPhase::COLLAPSING:
			// Move (fall) animations completed. Finalize positions and fill empty slots.
			finalizeFallingPieces();
			fillEmptySlots();
			break;

		case GameState::GravityPhase::AWAITING_FILL:
			// No move animations were needed (already at bottom), just fill.
			clearPendingSources();
			fillEmptySlots();
			break;
	}
}

/*
	Destroys all entities that were marked as matched (in comboItems).
	Called when AWAITING_COLLAPSE phase completes (destroy animations done).
*/
void GravitySystem::destroyMatchedEntities()
{
	for (EntityID entity : gameState.comboItems)
	{
		const PositionComponent* pos = gameState.entityManager->getComponent<PositionComponent>(entity);
		if (pos)
		{
			gameState.board->clearEntity(pos->x, pos->y);
		}
		gameState.entityManager->destroyEntity(entity);
	}
	gameState.comboItems.clear();
}

/*
	Computes which pieces should fall and starts their move animations.
	Transitions to COLLAPSING (if there are moves) or AWAITING_FILL (if no moves but needs fill).
*/
void GravitySystem::startCollapse()
{
	GravityResult gravityResult = GravityLogic::computeCollapse(*gameState.board, *gameState.entityManager);

	// Queue move animations for each falling piece
	for (const auto& move : gravityResult.moves)
	{
		gameState.animationSystem->addMoveAnimation(*gameState.entityManager, move.entity,
		                                            move.fromX, move.fromY, move.toX, move.toY);

		SwapData data;
		data.from = { move.fromX, move.fromY };
		data.to = { move.toX, move.toY };
		gameState.swaps.push(data);
	}

	// Transition to next phase
	if (gravityResult.moves.size() > 0)
	{
		// Pieces are falling - wait for animations, then finalize
		gameState.gravityPhase = GameState::GravityPhase::COLLAPSING;
	}
	else if (gravityResult.needsFill)
	{
		// No pieces to animate, but board needs new candies
		gameState.gravityPhase = GameState::GravityPhase::AWAITING_FILL;
	}
	else
	{
		// Nothing more to do - return to idle
		gameState.gravityPhase = GameState::GravityPhase::IDLE;
	}
}

/*
	Finalizes positions after move (fall) animations complete.
	Updates board grid, position components, and sprite positions.
	Then fills empty slots.
*/
void GravitySystem::finalizeFallingPieces()
{
	while (!gameState.swaps.empty())
	{
		SwapData d = gameState.swaps.front();
		gameState.swaps.pop();

		EntityID movingEnt = gameState.board->getEntity(d.from.x, d.from.y);
		gameState.board->setEntity(d.to.x, d.to.y, movingEnt);
		gameState.board->clearEntity(d.from.x, d.from.y);

		PositionComponent* pos = gameState.entityManager->getComponent<PositionComponent>(movingEnt);
		if (pos)
		{
			pos->x = d.to.x;
			pos->y = d.to.y;
		}

		SpriteComponent* sprite = gameState.entityManager->getComponent<SpriteComponent>(movingEnt);
		if (sprite)
		{
			Rect r = calculateRenderRectFromCoordinate(d.to.x, d.to.y);
			sprite->x = r.x;
			sprite->y = r.y;
		}
	}

	// After finalizing, always fill empty slots
	gameState.gravityPhase = GameState::GravityPhase::AWAITING_FILL;
}

/*
	Clears any pending source positions (defensive cleanup for AWAITING_FILL).
	This handles the edge case where collapse computed sources to clear.
*/
void GravitySystem::clearPendingSources()
{
	GravityResult tempResult = GravityLogic::computeCollapse(*gameState.board, *gameState.entityManager);
	for (const auto& pt : tempResult.sourcesToClear)
	{
		EntityID ent = gameState.board->getEntity(pt.x, pt.y);
		if (ent != NULL_ENTITY)
		{
			gameState.entityManager->destroyEntity(ent);
		}
		gameState.board->clearEntity(pt.x, pt.y);
	}
	// After clearing, stay in AWAITING_FILL to let fillEmptySlots() run
}

void GravitySystem::fillEmptySlots()
{
	std::vector<FillInfo> fills = GravityLogic::computeFills(*gameState.board);

	for (const auto& fill : fills)
	{
		EntityID entity = createGamePiece(fill.x, fill.y, fill.tex, true);
		gameState.board->setEntity(fill.x, fill.y, entity);
	}

	// Filling complete - return to idle
	gameState.gravityPhase = GameState::GravityPhase::IDLE;
}

EntityID GravitySystem::createGamePiece(int x, int y, const std::string& color, bool draggable)
{
	EntityID entity = gameState.entityManager->createEntity();

	PositionComponent pos;
	pos.x = x;
	pos.y = y;
	gameState.entityManager->addComponent(entity, pos);

	SpriteComponent sprite;
	sprite.textureId = color;
	Rect r = calculateRenderRectFromCoordinate(x, y);
	sprite.x = r.x;
	sprite.y = r.y;
	sprite.width = r.width;
	sprite.height = r.height;
	gameState.entityManager->addComponent(entity, sprite);

	MatchableComponent matchable;
	matchable.colorId = color;
	gameState.entityManager->addComponent(entity, matchable);

	DraggableComponent drag;
	drag.isDraggable = draggable;
	gameState.entityManager->addComponent(entity, drag);

	return entity;
}
