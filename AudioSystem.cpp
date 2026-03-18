#include "AudioSystem.h"
#include "AudioController.h"
#include "Events.h"
#include "EventBus.h"

AudioSystem::AudioSystem(GameState& state)
	: gameState(state)
{
	subscribeToEvents();
}

AudioSystem::~AudioSystem()
{
}

void AudioSystem::update(float dt)
{
	// Purely event-driven; all work happens in event callbacks
	// This method exists to satisfy ISystem interface
}

void AudioSystem::subscribeToEvents()
{
	if (!gameState.eventBus)
	{
		return;
	}

	// Subscribe to SwapExecutedEvent -> play swap sound
	gameState.eventBus->subscribe<SwapExecutedEvent>([](const IEvent& evt) {
		AudioController::getInstance().playSound("swap");
	});

	// Subscribe to MatchFoundEvent -> play match sound
	gameState.eventBus->subscribe<MatchFoundEvent>([](const IEvent& evt) {
		AudioController::getInstance().playSound("match");
	});
}