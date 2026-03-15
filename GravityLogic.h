#pragma once

#include "Board.h"
#include "ECS.h"
#include "Common.h"
#include "Components.h"
#include <vector>

/*
	Movement specification for an entity falling from one position to another.
*/
struct GravityMove
{
	EntityID entity;  // The entity that should move
	int fromX;
	int fromY;
	int toX;
	int toY;
};

/*
	Specification for a new entity to be created at an empty slot.
*/
struct FillInfo
{
	int x;
	int y;
	std::string tex;
};

/*
	Result of a gravity collapse computation.
	Contains all movements needed, source positions to clear, and whether filling is required.
*/
struct GravityResult
{
	std::vector<GravityMove> moves;
	std::vector<Point> sourcesToClear;  // positions that became empty after moves
	bool needsFill;
};

/*
	Stateless utility class for gravity/falling entity logic.
	Computes what should fall and what should be filled without
	modifying the board or triggering side effects.
*/
class GravityLogic
{
public:
	// Deleted constructor - static utility class
	GravityLogic() = delete;
	~GravityLogic() = delete;

	/*
		Computes all falling movements for the current board state.
		Scans from bottom-up, finds empty cells, and determines which
		entities above should fall into them.
		Returns GravityResult containing all moves and fill requirement.
	*/
	static GravityResult computeCollapse(const Board& board, const EntityManager& entityManager);

	/*
		Computes all fill specifications for empty cells on the board.
		Returns a vector of FillInfo specifying position and random color
		for each empty cell that should be filled.
	*/
	static std::vector<FillInfo> computeFills(const Board& board);

	/*
		Quick check: returns true if board has any empty cells.
	*/
	static bool hasEmptyCells(const Board& board);

	/*
		Computes how many empty cells exist in each column.
		Returns array of counts per column (index 0 = leftmost column).
	*/
	static std::array<int, GAME_MATRIX_SIZE_X> getEmptyCounts(const Board& board);

private:
	// Helper to check if position was already used as a source in this collapse
	static bool isSourceConsumed(int x, int y, const std::vector<GravityMove>& moves);
};
