#include "Board.h"

Board::Board()
{
	initialize();
}

Board::~Board()
{
	// Note: Entity lifecycle is managed by EntityManager, not Board
	// Board just stores references (EntityIDs)
}

void Board::initialize()
{
	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			grid[y][x] = NULL_ENTITY;
		}
	}
	for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
	{
		emptyColumnCounts[i] = 0;
	}
}

void Board::clear()
{
	// Just clear the grid references - EntityManager handles actual entity destruction
	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			grid[y][x] = NULL_ENTITY;
		}
	}
	for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
	{
		emptyColumnCounts[i] = 0;
	}
}

bool Board::isValidPosition(int x, int y) const
{
	return x >= 0 && x < GAME_MATRIX_SIZE_X && y >= 0 && y < GAME_MATRIX_SIZE_Y;
}

EntityID Board::getEntity(int x, int y) const
{
	if (!isValidPosition(x, y))
	{
		return NULL_ENTITY;
	}
	return grid[y][x];
}

void Board::setEntity(int x, int y, EntityID entity)
{
	if (!isValidPosition(x, y))
	{
		return;
	}
	grid[y][x] = entity;
}

void Board::clearEntity(int x, int y)
{
	if (!isValidPosition(x, y))
	{
		return;
	}
	grid[y][x] = NULL_ENTITY;
}

bool Board::isEmpty(int x, int y) const
{
	return getEntity(x, y) == NULL_ENTITY;
}

void Board::swapEntities(int fromX, int fromY, int toX, int toY)
{
	if (!isValidPosition(fromX, fromY) || !isValidPosition(toX, toY))
	{
		return;
	}

	EntityID temp = grid[fromY][fromX];
	grid[fromY][fromX] = grid[toY][toX];
	grid[toY][toX] = temp;
}

void Board::forEachEntity(std::function<void(EntityID)> callback) const
{
	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			if (grid[y][x] != NULL_ENTITY)
			{
				callback(grid[y][x]);
			}
		}
	}
}

void Board::forEachEntity(std::function<void(int, int, EntityID)> callback) const
{
	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			if (grid[y][x] != NULL_ENTITY)
			{
				callback(x, y, grid[y][x]);
			}
		}
	}
}

int Board::getEmptyCountInColumn(int col) const
{
	if (col < 0 || col >= GAME_MATRIX_SIZE_X)
	{
		return 0;
	}
	return emptyColumnCounts[col];
}

void Board::updateEmptyColumnCounts()
{
	for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
	{
		emptyColumnCounts[x] = 0;
		for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
		{
			if (grid[y][x] == NULL_ENTITY)
			{
				emptyColumnCounts[x]++;
			}
		}
	}
}
