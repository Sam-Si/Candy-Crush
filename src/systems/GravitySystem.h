#pragma once

#include "../ecs/ISystem.h"
#include "../game/GameState.h"
#include "../ecs/Components.h"

/*
	GravitySystem: Handles board collapse and empty slot filling.
	Processes after destroy animations complete.
*/
class GravitySystem : public ISystem
{
public:
	GravitySystem(GameState& state);
	~GravitySystem() override;

	// Handle collapse and fill operations when animations complete
	void update(float dt) override;

private:
	GameState& gameState;

	// --- Gravity cycle phase handlers ---
	// Each method handles one phase of the gravity state machine

	// AWAITING_COLLAPSE: Destroy matched entities from comboItems
	void destroyMatchedEntities();

	// Start falling: compute moves, queue animations, transition phase
	void startCollapse();

	// COLLAPSING: Finalize positions after fall animations complete
	void finalizeFallingPieces();

	// AWAITING_FILL: Clear any pending sources before filling
	void clearPendingSources();

	// Fill empty slots at the top with new entities (returns to IDLE)
	void fillEmptySlots();

	// Create a new game piece entity
	EntityID createGamePiece(int x, int y, const std::string& color, bool draggable);
};