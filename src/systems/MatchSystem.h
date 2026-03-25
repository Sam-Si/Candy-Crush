#pragma once

#include "../ecs/ISystem.h"
#include "../game/GameState.h"

/*
	MatchSystem: Detects matching entities and triggers destroy animations.
	Runs continuously to check for combos after moves/collapses.
*/
class MatchSystem : public ISystem
{
public:
	MatchSystem(GameState& state);
	~MatchSystem() override;

	// Check for matches and trigger destroy animations
	void update(float dt) override;

private:
	GameState& gameState;

	// Remove matched entities by adding destroy animations
	void removeComboItems(const std::set<EntityID>& matches);
};