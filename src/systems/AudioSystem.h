#pragma once

#include "../ecs/ISystem.h"
#include "../game/GameState.h"

/*
	AudioSystem: ECS system that handles game audio via EventBus subscription.
	Subscribes to MatchFoundEvent and SwapExecutedEvent to trigger sound effects.
	Purely event-driven; update() is a no-op.
*/
class AudioSystem : public ISystem
{
public:
	AudioSystem(GameState& state);
	~AudioSystem() override;

	// Event-driven system; no per-frame work needed
	void update(float dt) override;

private:
	GameState& gameState;

	// Subscribe to audio-related events
	void subscribeToEvents();
};