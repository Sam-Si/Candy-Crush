#include "MatchSystem.h"
#include "MatchLogic.h"
#include "AudioController.h"

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

	// Don't process matches if we're in the middle of collapse/fill
	if (gameState.collapseFlag || gameState.moveFlag || gameState.fillFlag)
	{
		return;
	}

	// Find all matches on the board
	std::set<EntityID> possibleCombos = MatchLogic::findMatches(*gameState.board, *gameState.entityManager);

	if (possibleCombos.size() > 0)
	{
		AudioController::getInstance().playSound("match");
		removeComboItems(possibleCombos);
	}
}

void MatchSystem::removeComboItems(const std::set<EntityID>& matches)
{
	gameState.comboItems.clear();

	for (EntityID entity : matches)
	{
		gameState.score += 1;
		if (gameState.uiManager)
		{
			gameState.uiManager->updateScore(gameState.score);
		}

		// Add destroy animation
		gameState.animationSystem->addDestroyAnimation(*gameState.entityManager, entity);
		gameState.comboItems.insert(entity);
	}

	if (gameState.comboItems.size() > 0)
	{
		gameState.collapseFlag = true;
	}
}