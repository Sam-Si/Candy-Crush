#pragma once

#include <string>
#include "Common.h"

/*
	Component definitions for the ECS architecture.
	All components are pure data structures with no behavior.
	No SDL dependencies - renderer-agnostic.
*/

/*
	Position component: Grid coordinates on the game board.
*/
struct PositionComponent
{
	int x = 0;  // Grid column (0 to GAME_MATRIX_SIZE_X-1)
	int y = 0;  // Grid row (0 to GAME_MATRIX_SIZE_Y-1)
};

/*
	Sprite component: Visual representation and rendering data.
	Uses generic float values - no SDL dependencies.
*/
struct SpriteComponent
{
	std::string textureId;  // Texture identifier (e.g., "blue", "red")
	float x = 0.0f;         // Screen X position
	float y = 0.0f;         // Screen Y position
	float width = 38.0f;    // Sprite width
	float height = 38.0f;   // Sprite height

	SpriteComponent() = default;

	SpriteComponent(const std::string& texId, float posX, float posY)
		: textureId(texId), x(posX), y(posY), width(38.0f), height(38.0f)
	{
	}
};

/*
	Matchable component: Identifies entities that can form matches.
	Stores the color/type used for match detection.
*/
struct MatchableComponent
{
	std::string colorId;  // Color identifier for matching (e.g., "blue", "red")
	
	MatchableComponent() = default;
	explicit MatchableComponent(const std::string& color) : colorId(color) {}
};

/*
	Draggable component: Identifies entities that can be dragged/swiped by player.
*/
struct DraggableComponent
{
	bool isDraggable = true;
};

/*
	Helper struct to initialize a game piece with all necessary components.
*/
struct GamePieceInit
{
	int x;
	int y;
	std::string color;
	bool draggable;
};

/*
	Animation types for AnimationComponent
*/
enum ANIMATION_TYPE { MOVE_ANIM, DESTROY_ANIM };

/*
	Animation component: Pure data for animation state.
	Used by AnimationSystem to interpolate sprite positions.
*/
struct AnimationComponent
{
	ANIMATION_TYPE type = MOVE_ANIM;  // Animation type
	int startX = 0;                   // Starting grid X
	int startY = 0;                   // Starting grid Y
	int targetX = 0;                  // Target grid X
	int targetY = 0;                  // Target grid Y
	int duration = 0;                 // Duration in milliseconds
	int elapsed = 0;                  // Elapsed time in milliseconds
};
