#pragma once

#include "Board.h"
#include "ECS.h"
#include "Components.h"
#include <set>

/*
	Stateless utility class for match detection logic.
	Provides methods to detect matching entities on the game board
	without modifying the board or triggering side effects.
*/
class MatchLogic
{
public:
	// Deleted constructor - static utility class
	MatchLogic() = delete;
	~MatchLogic() = delete;

	/*
		Checks if swapping two entities would result in a valid match (3+ in a row).
		Returns true if the swap creates at least one match of 3 or more entities.
	*/
	static bool checkIfValidMove(const Board& board, const EntityManager& entityManager, EntityID from, EntityID to);

	/*
		Scans the entire board and returns all entities that form matches.
		Checks both horizontal and vertical directions.
	*/
	static std::set<EntityID> findMatches(const Board& board, const EntityManager& entityManager);

	/*
		Finds all matches that involve the entity at position (x, y).
		Checks both horizontal and vertical through that position.
	*/
	static std::set<EntityID> findMatchesAt(const Board& board, const EntityManager& entityManager, int x, int y);

private:
	// Helper to count consecutive matching entities in a direction
	static int countMatchesInDirection(const Board& board, const EntityManager& entityManager, 
	                                   int startX, int startY, int deltaX, int deltaY, GAME_TEX matchTex);

	// Helper to collect all matching entities in a line
	static void collectMatchesInDirection(const Board& board, const EntityManager& entityManager,
	                                      int startX, int startY, int deltaX, int deltaY, GAME_TEX matchTex,
	                                      std::set<EntityID>& result);
	
	// Helper to get matchable color from entity
	static GAME_TEX getEntityColor(const EntityManager& entityManager, EntityID entity);
};
