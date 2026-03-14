#pragma once
#include "ECS.h"
#include "Components.h"
#include "Common.h"
#include <SDL2/SDL.h>

enum ANIMATION_TYPE { MOVE , DESTROY };

/*
	Animation object for an entity.
	Currently two kinds of animation supported which are destroy and move
	Animation is time based which means each animation type has a time in which the have to be completed
	and those times are defined in common.h header.
	So for example considering move, if a game entity has more way to go then it goes faster to be completed in time.
*/
class Animation
{
public:
	Animation();
	Animation(EntityManager* em, EntityID entity, ANIMATION_TYPE type);
	Animation(EntityManager* em, EntityID entity, ANIMATION_TYPE type, int target_x, int target_y);
	~Animation();

	void tick();					//update animation state
	bool isCompleted();				//checks if animation completed
	EntityID getSourceEntity();	
	void setAnimationStartTime(Uint32);

private:
	EntityManager*		entityManager;	//reference to entity manager
	EntityID			entity;			//source entity for the animation
	ANIMATION_TYPE		type;			//animation type
	int					target_x_coor;	//target x position for animation
	int					target_y_coor;	//target y position for animation
	SDL_Rect			targetRect;		
	SDL_Rect			originalRect;	//original position of the object 
	bool				completed;
	Uint32				startTime;

	void calculateTargetState();		//calculate the target position using animation type
};

