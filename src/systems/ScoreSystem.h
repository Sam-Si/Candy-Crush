#pragma once

#include "../ecs/ISystem.h"
#include "../game/GameState.h"

/*
	ScoreSystem: ECS system that handles score updates via EventBus subscription.
	Subscribes to MatchFoundEvent to update game score and UI.
	Purely event-driven; update() is a no-op.
*/
class ScoreSystem : public ISystem
{
public:
	ScoreSystem(GameState& state);
	~ScoreSystem() override;

	// Event-driven system; no per-frame work needed
	void update(float dt) override;

private:
	GameState& gameState;

	// Subscribe to score-related events
	void subscribeToEvents();
};