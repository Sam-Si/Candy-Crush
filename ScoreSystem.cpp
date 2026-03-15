#include "ScoreSystem.h"
#include "Events.h"
#include "EventBus.h"

ScoreSystem::ScoreSystem(GameState& state)
	: gameState(state)
{
	subscribeToEvents();
}

ScoreSystem::~ScoreSystem()
{
}

void ScoreSystem::update(float dt)
{
	// Purely event-driven; all work happens in event callbacks
	// This method exists to satisfy ISystem interface
}

void ScoreSystem::subscribeToEvents()
{
	if (!gameState.eventBus)
	{
		return;
	}

	// Subscribe to MatchFoundEvent -> update score
	gameState.eventBus->subscribe<MatchFoundEvent>([this](const IEvent& evt) {
		// Update score based on matched entities
		const Event<MatchFoundEvent>& matchEvent = static_cast<const Event<MatchFoundEvent>&>(evt);
		const auto& matched = matchEvent.data().matchedEntities;
		
		gameState.score += static_cast<int>(matched.size());
		
		// Update UI
		if (gameState.uiManager)
		{
			gameState.uiManager->updateScore(gameState.score);
		}
	});
}