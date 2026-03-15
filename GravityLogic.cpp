#include "GravityLogic.h"
#include "Common.h"
#include <array>

GravityResult GravityLogic::computeCollapse(const Board& board, const EntityManager& entityManager)
{
	GravityResult result;
	result.moves.clear();
	result.sourcesToClear.clear();
	result.needsFill = false;

	// For each column, collect non-empty entities and compute where they should end up
	for (int col = 0; col < GAME_MATRIX_SIZE_X; col++)
	{
		// Collect all non-empty entities in this column from bottom to top
		std::vector<EntityID> entities;
		for (int row = GAME_MATRIX_SIZE_Y - 1; row >= 0; row--)
		{
			EntityID ent = board.getEntity(col, row);
			if (ent != NULL_ENTITY)
			{
				entities.push_back(ent);
			}
		}

		// Get positions for each entity
		std::vector<int> originalRows;
		for (EntityID ent : entities)
		{
			const PositionComponent* pos = entityManager.getComponent<PositionComponent>(ent);
			if (pos)
			{
				originalRows.push_back(pos->y);
			}
			else
			{
				originalRows.push_back(0);
			}
		}

		// Entities should be stacked at bottom (GAME_MATRIX_SIZE_Y - 1 down)
		// Compute moves: entity at original row -> new row
		int newRow = GAME_MATRIX_SIZE_Y - 1;
		for (size_t i = 0; i < entities.size(); i++)
		{
			EntityID ent = entities[i];
			int originalRow = originalRows[i];
			
			if (originalRow != newRow)
			{
				// This entity needs to move - record source position to clear
				GravityMove move = { ent, col, originalRow, col, newRow };
				result.moves.push_back(move);
				result.sourcesToClear.push_back({ col, originalRow });
			}
			newRow--;
		}

		// Count empty slots at top (newRow went below 0 means all filled, else empties)
		int emptiesAtTop = (GAME_MATRIX_SIZE_Y - 1) - newRow;
		// But we also need to count original empties at top
		int originalEmptyCount = 0;
		for (int row = 0; row < GAME_MATRIX_SIZE_Y; row++)
		{
			if (board.isEmpty(col, row))
			{
				originalEmptyCount++;
			}
		}
		if (originalEmptyCount > 0 || emptiesAtTop > 0)
		{
			result.needsFill = true;
		}
	}

	return result;
}

std::vector<FillInfo> GravityLogic::computeFills(const Board& board)
{
	std::vector<FillInfo> fills;

	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			if (board.isEmpty(x, y))
			{
				std::string tex = getRandomColorId();
				FillInfo info = { x, y, tex };
				fills.push_back(info);
			}
		}
	}

	return fills;
}

bool GravityLogic::hasEmptyCells(const Board& board)
{
	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			if (board.isEmpty(x, y))
			{
				return true;
			}
		}
	}
	return false;
}

std::array<int, GAME_MATRIX_SIZE_X> GravityLogic::getEmptyCounts(const Board& board)
{
	std::array<int, GAME_MATRIX_SIZE_X> counts;
	for (int col = 0; col < GAME_MATRIX_SIZE_X; col++)
	{
		counts[col] = 0;
		for (int row = 0; row < GAME_MATRIX_SIZE_Y; row++)
		{
			if (board.isEmpty(col, row))
			{
				counts[col]++;
			}
		}
	}
	return counts;
}

bool GravityLogic::isSourceConsumed(int x, int y, const std::vector<GravityMove>& moves)
{
	for (const auto& move : moves)
	{
		if (move.fromX == x && move.fromY == y)
		{
			return true;
		}
	}
	return false;
}
