#include "GravitySystem.h"
#include "GravityLogic.h"

GravitySystem::GravitySystem(GameState& state)
	: gameState(state)
{
}

GravitySystem::~GravitySystem()
{
}

void GravitySystem::update(float dt)
{
	// Wait for animations to complete before processing
	if (gameState.isAnimating())
	{
		return;
	}

	// Process fill flag first (from previous collapse)
	if (gameState.fillFlag)
	{
		gameState.fillFlag = false;
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
		fillEmptySlots();
		return;
	}

	// Process collapse flag (destroy animations completed)
	if (gameState.collapseFlag)
	{
		// Destroy entities that were marked for removal
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
		gameState.collapseFlag = false;
		collapseBoard();
		return;
	}

	// Process move flag (gravity animations completed)
	if (gameState.moveFlag)
	{
		gameState.moveFlag = false;

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
		fillEmptySlots();
	}
}

void GravitySystem::collapseBoard()
{
	GravityResult gravityResult = GravityLogic::computeCollapse(*gameState.board, *gameState.entityManager);

	for (const auto& move : gravityResult.moves)
	{
		gameState.animationSystem->addMoveAnimation(*gameState.entityManager, move.entity,
		                                            move.fromX, move.fromY, move.toX, move.toY);

		SwapData data;
		data.from = { move.fromX, move.fromY };
		data.to = { move.toX, move.toY };
		gameState.swaps.push(data);
	}

	if (gravityResult.moves.size() > 0)
	{
		gameState.moveFlag = true;
	}
	else if (gravityResult.needsFill)
	{
		gameState.fillFlag = true;
	}
}

void GravitySystem::fillEmptySlots()
{
	std::vector<FillInfo> fills = GravityLogic::computeFills(*gameState.board);

	for (const auto& fill : fills)
	{
		EntityID entity = createGamePiece(fill.x, fill.y, fill.tex, true);
		gameState.board->setEntity(fill.x, fill.y, entity);
	}
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