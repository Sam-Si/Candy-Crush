#pragma once

#include <vector>
#include "ECS.h"

/*
	Events: Pure data structs for game event payloads.
	No logic, no virtual methods, no inheritance.
	Used to communicate what happened between systems.
*/

/*
	Emitted when a match is detected and candies are marked for removal.
	Contains all EntityIDs that formed the match.
*/
struct MatchFoundEvent
{
	std::vector<EntityID> matchedEntities;

	MatchFoundEvent() = default;

	explicit MatchFoundEvent(std::vector<EntityID> entities)
		: matchedEntities(std::move(entities))
	{
	}
};

/*
	Emitted when a valid swap is executed between two entities.
	Contains the two EntityIDs that were swapped.
*/
struct SwapExecutedEvent
{
	EntityID from = NULL_ENTITY;
	EntityID to = NULL_ENTITY;

	SwapExecutedEvent() = default;

	SwapExecutedEvent(EntityID f, EntityID t)
		: from(f), to(t)
	{
	}
};

/*
	Emitted when the game ends (time expires or explicit end).
	Contains the final score achieved.
*/
struct GameOverEvent
{
	int finalScore = 0;

	GameOverEvent() = default;

	explicit GameOverEvent(int score)
		: finalScore(score)
	{
	}
};
