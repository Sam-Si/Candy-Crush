#pragma once
#include "ECS.h"
#include "Components.h"
#include "AnimationController.h"
#include "Board.h"
#include "MatchLogic.h"
#include "GravityLogic.h"
#include "UIManager.h"
#include <vector>
#include <set>
#include <queue>
#include <iostream>
#include <fstream>
using namespace std;

/*
	Main controller responsible for game scene. Handles input, updates game state, uses animation manager etc.
	Uses ECS architecture: Entities are IDs, components store data, systems process logic.
*/
class SceneController
{

public:
	SceneController();
	~SceneController();

	void renderScene();							//render scene 
	void update();								//updates scene according to user input and animation state
	bool init();								//inits scene controller
	SDL_Renderer* getRenderer();
	EntityID pickEntity(SDL_Point&);			//picks game entity from a given point (game screen coordinates)
	void setBackground(SDL_Texture*);
	void setAnimationController(AnimationController* animController);	//animation controller injected from engine.
	void handleEvent(SDL_Event*);				//handles user inputs

	void generateLevel();						//generate level 

	void performMove(EntityID from, EntityID to);						//if it is a valid swap then performs the swap
	void checkPossipleCombosOnBoard();								//checks all possible combos at that state.
	void removeComboItems(const std::set<EntityID>& matches);			//when combo found, it removes combo objects from board
	void collapseBoard();											//after a successful removal, collapse the objects to fill empty slots
	void fillEmptySlots();											//after collapsing, fill empty slots at top
	void cleanup();													
	void setGameStartTime(Uint32);
	bool checkIfTimeIsUp();	
	bool quit = false;

private:
	SDL_Window*		gameWindow = 0;
	SDL_Surface*	gameSurface = 0;

	SDL_Renderer*	renderer = NULL;
	EntityID		selectedEntity = NULL_ENTITY;		//picked entity
	SDL_Texture*	background = NULL;
	Board*			board = NULL;						//game board that holds EntityIDs
	EntityManager*	entityManager = nullptr;			//ECS entity manager
	bool			collapseFlag = false;
	bool			moveFlag = false;
	bool			fillFlag = false;
	bool			gameOver = false;
	bool			handleInput = true;
	AnimationController* animationController;			//animation controller reference
	UIManager* uiManager = nullptr;						//UI manager for HUD rendering

	void onMouseButtonDown(SDL_Event*);
	void onMouseButtonUp(SDL_Event*);
	void onMouseMotion(SDL_Event*);

	//to transfer data to update after waiting animation completed
	std::set<EntityID> comboItems;	 

	std::queue<SwapData> swaps;

	// Game state (HUD displays these values)
	Uint32			gameStartTime;
	int score = 0;
	
	// Helper methods
	EntityID createGamePiece(int x, int y, GAME_TEX color, bool draggable);
	void renderEntity(EntityID entity);
};
