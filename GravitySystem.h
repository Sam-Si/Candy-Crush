#pragma once

#include "ISystem.h"
#include "GameState.h"
#include "Components.h"

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

	// Collapse entities down to fill empty spaces
	void collapseBoard();

	// Fill empty slots at the top with new entities
	void fillEmptySlots();

	// Create a new game piece entity
	EntityID createGamePiece(int x, int y, const std::string& color, bool draggable);
};