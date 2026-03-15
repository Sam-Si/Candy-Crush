#include "AnimationSystem.h"
#include "Common.h"

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
}

void AnimationSystem::update(EntityManager& entityManager, float dt)
{
	const auto& animations = entityManager.getAllComponents<AnimationComponent>();
	
	for (const auto& pair : animations)
	{
		EntityID entity = pair.first;
		
		// Get mutable components
		AnimationComponent* anim = entityManager.getComponent<AnimationComponent>(entity);
		SpriteComponent* sprite = entityManager.getComponent<SpriteComponent>(entity);
		
		if (!anim || !sprite)
		{
			continue;
		}
		
		// Update elapsed time
		anim->elapsed += static_cast<int>(dt);
		
		// Calculate target position based on grid coordinates
		Rect targetRect = calculateRenderRectFromCoordinate(anim->targetX, anim->targetY);
		Rect startRect = calculateRenderRectFromCoordinate(anim->startX, anim->startY);
		
		// Get original dimensions for move animation
		float originalW = 38.0f;
		float originalH = 38.0f;
		
		// Check if animation is complete
		if (anim->elapsed >= anim->duration)
		{
			// Animation complete - apply final state
			completeAnimation(entityManager, entity, anim, sprite);
			continue;
		}
		
		// Calculate interpolation factor (0.0 to 1.0)
		float t = static_cast<float>(anim->elapsed) / static_cast<float>(anim->duration);
		
		if (anim->type == DESTROY_ANIM)
		{
			// Destroy animation: shrink to center point
			float centerX = startRect.x + originalW / 2.0f;
			float centerY = startRect.y + originalH / 2.0f;
			
			// Width and height go to 0
			float newW = originalW * (1.0f - t);
			float newH = originalH * (1.0f - t);
			
			// Position shifts to center
			float newX = centerX - newW / 2.0f;
			float newY = centerY - newH / 2.0f;
			
			sprite->x = newX;
			sprite->y = newY;
			sprite->width = newW;
			sprite->height = newH;
		}
		else // MOVE_ANIM
		{
			// Move animation: interpolate position
			float newX = startRect.x + (targetRect.x - startRect.x) * t;
			float newY = startRect.y + (targetRect.y - startRect.y) * t;
			
			sprite->x = newX;
			sprite->y = newY;
			sprite->width = originalW;
			sprite->height = originalH;
		}
	}
}

bool AnimationSystem::hasAnimations(EntityManager& entityManager) const
{
	return getAnimationCount(entityManager) > 0;
}

int AnimationSystem::getAnimationCount(EntityManager& entityManager) const
{
	return static_cast<int>(entityManager.getAllComponents<AnimationComponent>().size());
}

void AnimationSystem::addMoveAnimation(EntityManager& entityManager, EntityID entity, 
                                       int fromX, int fromY, int toX, int toY)
{
	AnimationComponent anim;
	anim.type = MOVE_ANIM;
	anim.startX = fromX;
	anim.startY = fromY;
	anim.targetX = toX;
	anim.targetY = toY;
	anim.duration = MOVE_ANIMATION_TIME;
	anim.elapsed = 0;
	
	entityManager.addComponent(entity, anim);
}

void AnimationSystem::addDestroyAnimation(EntityManager& entityManager, EntityID entity)
{
	PositionComponent* pos = entityManager.getComponent<PositionComponent>(entity);
	if (!pos)
	{
		return;
	}
	
	AnimationComponent anim;
	anim.type = DESTROY_ANIM;
	anim.startX = pos->x;
	anim.startY = pos->y;
	anim.targetX = pos->x;
	anim.targetY = pos->y;
	anim.duration = DESTROY_ANIMATION_TIME;
	anim.elapsed = 0;
	
	entityManager.addComponent(entity, anim);
}

void AnimationSystem::completeAnimation(EntityManager& entityManager, EntityID entity,
                                        AnimationComponent* anim, SpriteComponent* sprite)
{
	// Apply final state based on animation type
	if (anim->type == DESTROY_ANIM)
	{
		// For destroy, set to zero-size at center
		Rect finalRect = calculateRenderRectFromCoordinate(anim->startX, anim->startY);
		sprite->x = finalRect.x + finalRect.width / 2.0f;
		sprite->y = finalRect.y + finalRect.height / 2.0f;
		sprite->width = 0.0f;
		sprite->height = 0.0f;
	}
	else // MOVE_ANIM
	{
		// For move, set to target position
		Rect finalRect = calculateRenderRectFromCoordinate(anim->targetX, anim->targetY);
		sprite->x = finalRect.x;
		sprite->y = finalRect.y;
		sprite->width = finalRect.width;
		sprite->height = finalRect.height;
	}
	
	// Remove the animation component
	entityManager.removeComponent<AnimationComponent>(entity);
}