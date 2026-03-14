#pragma once

#include "ECS.h"
#include "Common.h"
#include <SDL2/SDL.h>
#include <functional>

/*
	Board encapsulates the 2D game grid management.
	It stores EntityID in an 8x8 grid and provides
	methods for accessing, modifying, and iterating over grid cells.
*/
class Board
{
public:
	Board();
	~Board();

	// Entity access
	EntityID getEntity(int x, int y) const;
	void setEntity(int x, int y, EntityID entity);
	void clearEntity(int x, int y);
	bool isEmpty(int x, int y) const;

	// Grid operations
	void swapEntities(int fromX, int fromY, int toX, int toY);

	// Iteration
	void forEachEntity(std::function<void(EntityID)> callback) const;
	void forEachEntity(std::function<void(int, int, EntityID)> callback) const;

	// Lifecycle
	void clear();
	void initialize();

	// Column tracking for collapse operations
	int getEmptyCountInColumn(int col) const;
	void updateEmptyColumnCounts();
	const int* getEmptyColumnCounts() const { return emptyColumnCounts; }

private:
	EntityID grid[GAME_MATRIX_SIZE_Y][GAME_MATRIX_SIZE_X];
	int emptyColumnCounts[GAME_MATRIX_SIZE_X]; // tracks empty slots per column

	// Bounds checking
	bool isValidPosition(int x, int y) const;
};
