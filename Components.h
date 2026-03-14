#pragma once

#include "Common.h"
#include <SDL2/SDL.h>

/*
	Component definitions for the ECS architecture.
	All components are pure data structures with no behavior.
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
*/
struct SpriteComponent
{
	GAME_TEX tex = BLUE_OBJ;  // Texture type/color
	SDL_Rect targetRect;      // Screen position and dimensions for rendering

	SpriteComponent()
	{
		targetRect = { 0, 0, STONE_WIDTH, STONE_HEIGHT };
	}

	SpriteComponent(GAME_TEX texture, int x, int y)
	{
		tex = texture;
		targetRect = { x, y, STONE_WIDTH, STONE_HEIGHT };
	}
};

/*
	Matchable component: Identifies entities that can form matches.
	Stores the color/type used for match detection.
*/
struct MatchableComponent
{
	GAME_TEX color = BLUE_OBJ;  // Color for matching (BLUE_OBJ, RED_OBJ, etc.)
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
	GAME_TEX color;
	bool draggable;
};
