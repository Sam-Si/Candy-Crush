/*
Coordinate system
-------------------------------------------------
| 0,0 | 1,0 | 2,0 | 3,0 | 4,0 | 5,0 | 6,0 | 7,0 |
|-------------------------------------------------
| 0,1 | 1,1 | 2,1 | 3,1 | 4, 1| ..................
...................................................
....................................................

*/

#pragma once
#include <time.h>
#include <stdlib.h>
#include <string>
#include <SDL2/SDL.h>

enum GAME_TEX{ BACKGROUND, GAMEOVER,BLUE_OBJ, GREEN_OBJ, PURPLE_OBJ, RED_OBJ, YELLOW_OBJ };

const int GAME_WITDH			= 755;	//game screen width
const int GAME_HEIGHT			= 600;  //game screen height
const int GAME_DURATION			= 60;	//total game duration.1min by default
const int RENDER_AREA_X_POS		= 316;  //render area position  x
const int RENDER_AREA_Y_POS		= 93;   //render area position  y
const int RENDER_AREA_WIDTH		= 362;  //render area position  width
const int RENDER_AREA_HEIGHT	= 347;  //render area position  height

const int STONE_WIDTH			= 38;	//game object width
const int STONE_HEIGHT			= 38;   //game object height

const int GAME_MATRIX_SIZE_X	= 8;
const int GAME_MATRIX_SIZE_Y	= 8;

const int DESTROY_ANIMATION_TIME = 500; // 0.5 second
const int MOVE_ANIMATION_TIME	 = 400; //0.4 second

const float MS_60_MS = 16.66f ; //60 FPS

const int GAP_X = (RENDER_AREA_WIDTH - (GAME_MATRIX_SIZE_X * STONE_WIDTH)) / (GAME_MATRIX_SIZE_X - 1); //horizontal gap between game objects
const int GAP_Y = (RENDER_AREA_HEIGHT - (GAME_MATRIX_SIZE_Y * STONE_HEIGHT)) / (GAME_MATRIX_SIZE_Y - 1);//vertical gap between game objects

/*
	Game run on 8 x 8 grid by default.
	So when we give a coordinate to this function it calculates
	the exact rendering position on screen considering rendering area
*/
inline void calculateRenderPositionFromCoordinate(int x, int y, SDL_Rect* r)
{
	r->x = RENDER_AREA_X_POS + (x * STONE_WIDTH + (x * GAP_X));
	r->y = RENDER_AREA_Y_POS + (y * STONE_HEIGHT + (y * GAP_Y));
	r->w = STONE_WIDTH;
	r->h = STONE_HEIGHT;
}
/*
	Finds the coordinate on grid by using the game window coordinate
*/
inline void calculateCoordinateFromMousePositions(int x, int y, SDL_Point& p)
{
	//first find the coordinate relative to render screen
	int relative_x = x - RENDER_AREA_X_POS;
	int relative_y = y - RENDER_AREA_Y_POS;

	int k = relative_x - (relative_x / STONE_WIDTH -1) * GAP_X;
	int l = relative_y - (relative_y / STONE_HEIGHT -1) * GAP_Y;

	p.x = k / STONE_WIDTH;
	p.y = l / STONE_HEIGHT;
}

/*
	Generates a random color object for the game
	Uses +2 as we dnt want to get BACKGROUND and GAMEOVER screen just colors
	so we exlude them.
*/
inline GAME_TEX getRandomObj()
{	
	int color = rand() % 5 + 2; //exclude background
	GAME_TEX c = static_cast<GAME_TEX>(color);
	return c;
}

// String-based random color generator for new code
inline std::string getRandomColorId()
{
	static const std::string colors[] = { "blue", "green", "purple", "red", "yellow" };
	return colors[rand() % 5];
}

// Generic point struct - no SDL dependency
struct Point
{
	int x = 0;
	int y = 0;
	Point() = default;
	Point(int x_, int y_) : x(x_), y(y_) {}
};

/*
	Data structrue with 2 positions.
	We usually use it to store 2 positions those will be swap after animation compeletes.
*/
struct SwapData
{
	Point from;
	Point to;
};

// Generic rect struct - no SDL dependency  
struct Rect
{
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
	
	Rect() = default;
	Rect(float x_, float y_, float w_, float h_) : x(x_), y(y_), width(w_), height(h_) {}
};

/*
	SDL-free version: Calculate render position from grid coordinate.
	Returns a generic Rect.
*/
inline Rect calculateRenderRectFromCoordinate(int x, int y)
{
	Rect r;
	r.x = static_cast<float>(RENDER_AREA_X_POS + (x * STONE_WIDTH + (x * GAP_X)));
	r.y = static_cast<float>(RENDER_AREA_Y_POS + (y * STONE_HEIGHT + (y * GAP_Y)));
	r.width = static_cast<float>(STONE_WIDTH);
	r.height = static_cast<float>(STONE_HEIGHT);
	return r;
}

/*
	SDL-free version: Find grid coordinate from mouse position.
	Takes Point by reference for output.
*/
inline void calculateCoordinateFromMousePos(int mouseX, int mouseY, Point& out)
{
	int relative_x = mouseX - RENDER_AREA_X_POS;
	int relative_y = mouseY - RENDER_AREA_Y_POS;

	int k = relative_x - (relative_x / STONE_WIDTH - 1) * GAP_X;
	int l = relative_y - (relative_y / STONE_HEIGHT - 1) * GAP_Y;

	out.x = k / STONE_WIDTH;
	out.y = l / STONE_HEIGHT;
}