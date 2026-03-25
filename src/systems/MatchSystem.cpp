#include "MatchSystem.h"
#include "../game/MatchLogic.h"
#include "../events/Events.h"

MatchSystem::MatchSystem(GameState& state)
	: gameState(state)
{
}

MatchSystem::~MatchSystem()
{
}

void MatchSystem::update(float dt)
{
	// Skip if animations are playing - wait for them to complete
	if (gameState.isAnimating())
	{
		return;
	}

	// Don't process matches if gravity cycle is in progress
	// (prevents detecting matches while animations are still settling)
	if (gameState.isGravityCycleActive())
	{
		return;
	}

	// Find all matches on the board
	std::set<EntityID> possibleCombos = MatchLogic::findMatches(*gameState.board, *gameState.entityManager);

	if (possibleCombos.size() > 0)
	{
		// Publish MatchFoundEvent for decoupled handling
		if (gameState.eventBus)
		{
			std::vector<EntityID> matchesVector(possibleCombos.begin(), possibleCombos.end());
			gameState.eventBus->publish(MatchFoundEvent(std::move(matchesVector)));
		}
		removeComboItems(possibleCombos);
	}
}

void MatchSystem::removeComboItems(const std::set<EntityID>& matches)
{
	gameState.comboItems.clear();

	for (EntityID entity : matches)
	{
		// Note: Score is now handled by ScoreSystem via MatchFoundEvent
		// Add destroy animation
		gameState.animationSystem->addDestroyAnimation(*gameState.entityManager, entity);
		gameState.comboItems.insert(entity);
	}

	if (gameState.comboItems.size() > 0)
	{
		// Trigger gravity cycle: destroy animations will play, then collapse
		gameState.gravityPhase = GameState::GravityPhase::AWAITING_COLLAPSE;
	}
}