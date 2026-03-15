#pragma once

#include "ECS.h"
#include "Components.h"

/*
	AnimationSystem: Pure ECS system for processing animations.
	Iterates over all entities with AnimationComponent and SpriteComponent,
	updates sprite positions based on animation state.
*/
class AnimationSystem
{
public:
	AnimationSystem();
	~AnimationSystem();

	// Process all animations for a frame. Call this every update.
	void update(EntityManager& entityManager, float dt);

	// Check if any animations are currently playing
	bool hasAnimations(EntityManager& entityManager) const;

	// Get count of active animations
	int getAnimationCount(EntityManager& entityManager) const;

	// Add a move animation to an entity
	void addMoveAnimation(EntityManager& entityManager, EntityID entity, 
	                      int fromX, int fromY, int toX, int toY);

	// Add a destroy animation to an entity
	void addDestroyAnimation(EntityManager& entityManager, EntityID entity);

private:
	// Apply final animation state and remove the component
	void completeAnimation(EntityManager& entityManager, EntityID entity, 
	                       AnimationComponent* anim, SpriteComponent* sprite);
};