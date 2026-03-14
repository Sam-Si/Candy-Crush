#include "Animation.h"
#include "Common.h"
#include <iostream>

Animation::Animation()
	: entityManager(nullptr), entity(NULL_ENTITY), type(MOVE), target_x_coor(0), target_y_coor(0),
	  completed(false), startTime(0)
{
}

Animation::Animation(EntityManager* em, EntityID ent, ANIMATION_TYPE t)
	: entityManager(em), entity(ent), type(t), target_x_coor(0), target_y_coor(0),
	  completed(false), startTime(0)
{
	calculateTargetState();
}

Animation::Animation(EntityManager* em, EntityID ent, ANIMATION_TYPE t, int target_x, int target_y)
	: entityManager(em), entity(ent), type(t), target_x_coor(target_x), target_y_coor(target_y),
	  completed(false), startTime(0)
{
	calculateTargetState();
}

Animation::~Animation(){}

void Animation::calculateTargetState()
{
	if (!entityManager || entity == NULL_ENTITY)
	{
		return;
	}

	SpriteComponent* sprite = entityManager->getComponent<SpriteComponent>(entity);
	if (!sprite)
	{
		return;
	}

	calculateRenderPositionFromCoordinate(target_x_coor, target_y_coor, &targetRect);
	
	//if it is destroy animation then it will disappear on screen so that its w and h will go to 0 over time
	if (type == DESTROY)
	{
		targetRect.x = sprite->targetRect.x + STONE_WIDTH / 2;
		targetRect.y = sprite->targetRect.y + STONE_HEIGHT / 2;
		targetRect.h = 0;
		targetRect.w = 0;
	}
	else if (type == MOVE)
	{
		SDL_Rect r;
		calculateRenderPositionFromCoordinate(target_x_coor, target_y_coor, &r);
		targetRect.x = r.x;
		targetRect.y = r.y;
		targetRect.h = sprite->targetRect.h;
		targetRect.w = sprite->targetRect.w;
	}
	
	//save the original position of the objects so that they will be calculated for calculations
	originalRect.x = sprite->targetRect.x;
	originalRect.y = sprite->targetRect.y;
	originalRect.w = sprite->targetRect.w;
	originalRect.h = sprite->targetRect.h;	
}

/*
	Updates the animation.
*/
void Animation::tick()
{
	if (!entityManager || entity == NULL_ENTITY)
	{
		return;
	}

	SpriteComponent* sprite = entityManager->getComponent<SpriteComponent>(entity);
	if (!sprite)
	{
		return;
	}

	int new_x = 0;
	int new_y = 0;
	int new_w = 0;
	int new_h = 0;
	int animDuration = 0;

	//get the animation duration according to animation type
	if (type == DESTROY) animDuration = DESTROY_ANIMATION_TIME;
	if (type == MOVE) animDuration = MOVE_ANIMATION_TIME;

	//calculate elapsed time
	Uint32	elapsedTime = SDL_GetTicks() - startTime;

	//if elapsed time bigger then animation duration then we should finish the animation
	if (elapsedTime >= animDuration) //time is up!
	{
		new_x = targetRect.x;
		new_y = targetRect.y;
		new_w = targetRect.w;
		new_h = targetRect.h;
	}
	else{
		//calculate the positions according to time passed 		
		float dt = (float)elapsedTime / (float)animDuration;
		new_x = originalRect.x + ((targetRect.x -  originalRect.x) * dt);
		new_y = originalRect.y + ((targetRect.y - originalRect.y) * dt);
		new_h = originalRect.h + ((targetRect.h - originalRect.h) * dt);
		new_w = originalRect.w + ((targetRect.w - originalRect.w) * dt);
	}
	
	//update object positions with calculated ones
	sprite->targetRect.x = new_x;
	sprite->targetRect.y = new_y;
	sprite->targetRect.w = new_w;
	sprite->targetRect.h = new_h;
}

/*
	check if object reached to target position which means animation completed
*/
bool Animation::isCompleted()
{
	if (!entityManager || entity == NULL_ENTITY)
	{
		return true;
	}

	SpriteComponent* sprite = entityManager->getComponent<SpriteComponent>(entity);
	if (!sprite)
	{
		return true;
	}

	return (sprite->targetRect.x == targetRect.x  && sprite->targetRect.y == targetRect.y &&
			sprite->targetRect.w == targetRect.w && sprite->targetRect.h == targetRect.h);
}

EntityID Animation::getSourceEntity()
{
	return entity;
}

void Animation::setAnimationStartTime(Uint32 t)
{
	startTime = t;
}