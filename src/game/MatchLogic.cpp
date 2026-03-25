#include "MatchLogic.h"

/*
	Helper to get matchable color from entity.
*/
std::string MatchLogic::getEntityColor(const EntityManager& entityManager, EntityID entity)
{
	if (entity == NULL_ENTITY)
	{
		return ""; // Invalid color
	}
	
	const MatchableComponent* matchable = entityManager.getComponent<MatchableComponent>(entity);
	if (matchable)
	{
		return matchable->colorId;
	}
	
	// Fallback to sprite component if no matchable component
	const SpriteComponent* sprite = entityManager.getComponent<SpriteComponent>(entity);
	if (sprite)
	{
		return sprite->textureId;
	}
	
	return "";
}

/*
	Checks if swapping two entities would result in a valid match.
	This temporarily checks what would happen if 'from' entity moved to 'to' position.
*/
bool MatchLogic::checkIfValidMove(const Board& board, const EntityManager& entityManager, EntityID from, EntityID to)
{
	if (from == NULL_ENTITY || to == NULL_ENTITY)
	{
		return false;
	}

	const PositionComponent* fromPos = entityManager.getComponent<PositionComponent>(from);
	const PositionComponent* toPos = entityManager.getComponent<PositionComponent>(to);
	
	if (!fromPos || !toPos)
	{
		return false;
	}

	int horizontalCount = 1;
	int verticalCount = 1;

	int x = toPos->x;
	int y = toPos->y;

	// Get the color of the entity being moved (the 'from' entity)
	std::string fromColor = getEntityColor(entityManager, from);

	// Check horizontal combos to the left of the target position
	for (int i = x - 1; i >= 0; i--)
	{
		EntityID ent = board.getEntity(i, y);
		// Skip the original position of 'from' entity (it's moving away)
		if (i == fromPos->x)
		{
			continue;
		}
		if (ent != NULL_ENTITY && getEntityColor(entityManager, ent) == fromColor)
		{
			horizontalCount++;
		}
		else
		{
			break;
		}
	}

	// Check horizontal combos to the right of the target position
	for (int i = x + 1; i < GAME_MATRIX_SIZE_X; i++)
	{
		EntityID ent = board.getEntity(i, y);
		// Skip the original position of 'from' entity (it's moving away)
		if (i == fromPos->x)
		{
			continue;
		}
		if (ent != NULL_ENTITY && getEntityColor(entityManager, ent) == fromColor)
		{
			horizontalCount++;
		}
		else
		{
			break;
		}
	}

	// Horizontal combo of 3+ is enough to validate the move
	if (horizontalCount >= 3)
	{
		return true;
	}

	// Check vertical combos upward from the target position
	for (int i = y - 1; i >= 0; i--)
	{
		EntityID ent = board.getEntity(x, i);
		// Skip the original position of 'from' entity (it's moving away)
		if (i == fromPos->y)
		{
			continue;
		}
		if (ent != NULL_ENTITY && getEntityColor(entityManager, ent) == fromColor)
		{
			verticalCount++;
		}
		else
		{
			break;
		}
	}

	// Check vertical combos downward from the target position
	for (int i = y + 1; i < GAME_MATRIX_SIZE_Y; i++)
	{
		EntityID ent = board.getEntity(x, i);
		// Skip the original position of 'from' entity (it's moving away)
		if (i == fromPos->y)
		{
			continue;
		}
		if (ent != NULL_ENTITY && getEntityColor(entityManager, ent) == fromColor)
		{
			verticalCount++;
		}
		else
		{
			break;
		}
	}

	// Vertical combo of 3+ validates the move
	return verticalCount >= 3;
}

/*
	Scans the entire board for all horizontal and vertical matches.
	Returns a set of all entities that are part of any match (3+ in a row).
*/
std::set<EntityID> MatchLogic::findMatches(const Board& board, const EntityManager& entityManager)
{
	std::set<EntityID> matches;

	// Check horizontal matches
	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X - 2; x++)
		{
			EntityID entity = board.getEntity(x, y);
			EntityID next1 = board.getEntity(x + 1, y);
			EntityID next2 = board.getEntity(x + 2, y);

			if (entity == NULL_ENTITY || next1 == NULL_ENTITY || next2 == NULL_ENTITY)
			{
				continue;
			}

			std::string color0 = getEntityColor(entityManager, entity);
			std::string color1 = getEntityColor(entityManager, next1);
			std::string color2 = getEntityColor(entityManager, next2);

			// Found a potential match of 3
			if (color1 == color0 && color2 == color0)
			{
				matches.insert(entity);
				matches.insert(next1);
				matches.insert(next2);

				// Check for extended match (4+ in a row)
				if (x + 3 >= GAME_MATRIX_SIZE_X)
				{
					break;
				}

				int nextIndex = x + 3;
				EntityID nextEnt = board.getEntity(nextIndex, y);
				while (nextIndex < GAME_MATRIX_SIZE_X && nextEnt != NULL_ENTITY && 
				       getEntityColor(entityManager, nextEnt) == color0)
				{
					matches.insert(nextEnt);
					nextIndex++;
					nextEnt = board.getEntity(nextIndex, y);
				}
				x = nextIndex;
			}
		}
	}

	// Check vertical matches
	for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
	{
		for (int y = 0; y < GAME_MATRIX_SIZE_Y - 2; y++)
		{
			EntityID entity = board.getEntity(x, y);
			EntityID next1 = board.getEntity(x, y + 1);
			EntityID next2 = board.getEntity(x, y + 2);

			if (entity == NULL_ENTITY || next1 == NULL_ENTITY || next2 == NULL_ENTITY)
			{
				continue;
			}

			std::string color0 = getEntityColor(entityManager, entity);
			std::string color1 = getEntityColor(entityManager, next1);
			std::string color2 = getEntityColor(entityManager, next2);

			// Found a potential match of 3
			if (color1 == color0 && color2 == color0)
			{
				matches.insert(entity);
				matches.insert(next1);
				matches.insert(next2);

				// Check for extended match (4+ in a column)
				if (y + 3 >= GAME_MATRIX_SIZE_Y)
				{
					break;
				}

				int nextIndex = y + 3;
				EntityID nextEnt = board.getEntity(x, nextIndex);
				while (nextIndex < GAME_MATRIX_SIZE_Y && nextEnt != NULL_ENTITY && 
				       getEntityColor(entityManager, nextEnt) == color0)
				{
					matches.insert(nextEnt);
					nextIndex++;
					nextEnt = board.getEntity(x, nextIndex);
				}
				y = nextIndex;
			}
		}
	}

	return matches;
}

/*
	Finds all matches that involve the entity at position (x, y).
	This checks both horizontal and vertical lines through that position.
*/
std::set<EntityID> MatchLogic::findMatchesAt(const Board& board, const EntityManager& entityManager, int x, int y)
{
	std::set<EntityID> matches;

	EntityID entity = board.getEntity(x, y);
	if (entity == NULL_ENTITY)
	{
		return matches;
	}

	std::string matchTex = getEntityColor(entityManager, entity);

	// Check horizontal line
	int horizontalCount = 1;
	horizontalCount += countMatchesInDirection(board, entityManager, x, y, -1, 0, matchTex); // left
	horizontalCount += countMatchesInDirection(board, entityManager, x, y, 1, 0, matchTex);  // right

	if (horizontalCount >= 3)
	{
		matches.insert(entity);
		collectMatchesInDirection(board, entityManager, x, y, -1, 0, matchTex, matches); // left
		collectMatchesInDirection(board, entityManager, x, y, 1, 0, matchTex, matches);  // right
	}

	// Check vertical line
	int verticalCount = 1;
	verticalCount += countMatchesInDirection(board, entityManager, x, y, 0, -1, matchTex); // up
	verticalCount += countMatchesInDirection(board, entityManager, x, y, 0, 1, matchTex);  // down

	if (verticalCount >= 3)
	{
		matches.insert(entity);
		collectMatchesInDirection(board, entityManager, x, y, 0, -1, matchTex, matches); // up
		collectMatchesInDirection(board, entityManager, x, y, 0, 1, matchTex, matches);  // down
	}

	return matches;
}

/*
	Helper: Counts consecutive matching entities starting from (startX, startY)
	moving in direction (deltaX, deltaY).
*/
int MatchLogic::countMatchesInDirection(const Board& board, const EntityManager& entityManager,
                                        int startX, int startY, int deltaX, int deltaY, const std::string& matchTex)
{
	int count = 0;
	int x = startX + deltaX;
	int y = startY + deltaY;

	while (x >= 0 && x < GAME_MATRIX_SIZE_X && y >= 0 && y < GAME_MATRIX_SIZE_Y)
	{
		EntityID ent = board.getEntity(x, y);
		if (ent != NULL_ENTITY && getEntityColor(entityManager, ent) == matchTex)
		{
			count++;
			x += deltaX;
			y += deltaY;
		}
		else
		{
			break;
		}
	}

	return count;
}

/*
	Helper: Collects all matching entities starting from (startX, startY)
	moving in direction (deltaX, deltaY) and adds them to the result set.
*/
void MatchLogic::collectMatchesInDirection(const Board& board, const EntityManager& entityManager,
                                           int startX, int startY, int deltaX, int deltaY, const std::string& matchTex,
                                           std::set<EntityID>& result)
{
	int x = startX + deltaX;
	int y = startY + deltaY;

	while (x >= 0 && x < GAME_MATRIX_SIZE_X && y >= 0 && y < GAME_MATRIX_SIZE_Y)
	{
		EntityID ent = board.getEntity(x, y);
		if (ent != NULL_ENTITY && getEntityColor(entityManager, ent) == matchTex)
		{
			result.insert(ent);
			x += deltaX;
			y += deltaY;
		}
		else
		{
			break;
		}
	}
}
