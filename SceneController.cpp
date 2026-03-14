#include "SceneController.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>
#include <map>
#include <queue>
#include <sstream>
#include <memory>
using namespace std;

SceneController::SceneController() 
	: entityManager(new EntityManager())
{ }

SceneController::~SceneController()
{ 
	delete entityManager;
}

EntityID SceneController::createGamePiece(int x, int y, GAME_TEX color, bool draggable)
{
	EntityID entity = entityManager->createEntity();
	
	PositionComponent pos;
	pos.x = x;
	pos.y = y;
	entityManager->addComponent(entity, pos);
	
	SpriteComponent sprite;
	sprite.tex = color;
	calculateRenderPositionFromCoordinate(x, y, &sprite.targetRect);
	entityManager->addComponent(entity, sprite);
	
	MatchableComponent matchable;
	matchable.color = color;
	entityManager->addComponent(entity, matchable);
	
	DraggableComponent drag;
	drag.isDraggable = draggable;
	entityManager->addComponent(entity, drag);
	
	return entity;
}

bool SceneController::init()
{
	bool success = true;
	
	board = new Board();
	board->initialize();

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "Error on init SDL" << endl;
		success = false;
	}
	else
	{
		gameWindow = SDL_CreateWindow("KingCrash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GAME_WITDH, GAME_HEIGHT, SDL_WINDOW_SHOWN);
		if (gameWindow == NULL)
		{
			cout << "Game window could not be created" << endl;
			success = false;
		}
		else
		{
			Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
#ifdef SOFTWARE_ACC
			rendererFlags = SDL_RENDERER_SOFTWARE;
#endif
			renderer = SDL_CreateRenderer(gameWindow, -1, rendererFlags);
			if (renderer == NULL)
			{
				cout << "cannot create renderer" << endl;
				success = false;
			}
			else
			{
				int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\\n", IMG_GetError());
					success = false;
				}
				else
				{
					gameSurface = SDL_GetWindowSurface(gameWindow);
					if (TTF_Init() == -1)
					{
						printf("SDL_ttf could not initialize! SDL_ttf Error: %s\\n", TTF_GetError());
						success = false;
					}
				}
			}
		}
	}

	uiManager = new UIManager();
	bool uiResult = uiManager->init(renderer);
	
	return success && uiResult;
}

void SceneController::setBackground(SDL_Texture* bg)
{
	background = bg;
}

SDL_Renderer* SceneController::getRenderer()
{
	return renderer;
}

void SceneController::update()
{   
    if (animationController->checkAnyAnimation() == false)
    {
        handleInput = true;
    }
    else{
        handleInput = false;
    }

    if (checkIfTimeIsUp())
    {
        if (!gameOver)
        {
            uiManager->showGameOver();
        }       
        gameOver = true;
        return;
    }
    
    uiManager->updateTime(gameStartTime);
    animationController->tick();
    
    if (animationController->checkAnyAnimation() == false && fillFlag)
    {
        fillFlag = false;
        GravityResult tempResult = GravityLogic::computeCollapse(*board, *entityManager);
        for (const auto& pt : tempResult.sourcesToClear)
        {
            EntityID ent = board->getEntity(pt.x, pt.y);
            if (ent != NULL_ENTITY)
            {
                entityManager->destroyEntity(ent);
            }
            board->clearEntity(pt.x, pt.y);
        }
        fillEmptySlots();
    }

    if (animationController->checkAnyAnimation() == false && collapseFlag)
    {
        for (auto it = comboItems.begin(); it != comboItems.end(); ++it)
        {
            const PositionComponent* pos = entityManager->getComponent<PositionComponent>(*it);
            if (pos)
            {
                board->clearEntity(pos->x, pos->y);
            }
            entityManager->destroyEntity(*it);
        }
        comboItems.clear();
        collapseFlag = false;
        collapseBoard();
    }

    if (animationController->checkAnyAnimation() == false && moveFlag)
    {
        moveFlag = false;
        
        while (!swaps.empty())
        {
            SwapData d = swaps.front();
            swaps.pop();

            EntityID movingEnt = board->getEntity(d.from.x, d.from.y);
            board->setEntity(d.to.x, d.to.y, movingEnt);
            board->clearEntity(d.from.x, d.from.y);
            
            PositionComponent* pos = entityManager->getComponent<PositionComponent>(movingEnt);
            if (pos)
            {
                pos->x = d.to.x;
                pos->y = d.to.y;
            }
            
            SpriteComponent* sprite = entityManager->getComponent<SpriteComponent>(movingEnt);
            if (sprite)
            {
                calculateRenderPositionFromCoordinate(d.to.x, d.to.y, &sprite->targetRect);
            }
        }
        fillEmptySlots();
    }
}

void SceneController::handleEvent(SDL_Event* event)
{
    if (gameOver)
    {
        if (event->type == SDL_KEYDOWN)
        {
            if (event->key.keysym.sym == SDLK_RETURN)
            {
                generateLevel();
                handleInput = true;
                gameOver = false;
                gameStartTime = SDL_GetTicks();
            }
            else if (event->key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            }
        }
    }

    if (checkIfTimeIsUp() || handleInput == false) return;

    switch (event->type)
    {
        case SDL_MOUSEBUTTONDOWN:
            onMouseButtonDown(event);
            break;
        case SDL_MOUSEBUTTONUP:
            onMouseButtonUp(event);
            break;
        case SDL_MOUSEMOTION:
            onMouseMotion(event);
            break;
        default:
            break;
    }
}

EntityID SceneController::pickEntity(SDL_Point& p)
{
    SDL_Point coordinate;
    calculateCoordinateFromMousePositions(p.x, p.y, coordinate);

    if (coordinate.x < 0 || coordinate.y < 0)
    {
        return NULL_ENTITY;
    }
    
    EntityID ent = board->getEntity(coordinate.x, coordinate.y);
    if (ent != NULL_ENTITY)
    {
        DraggableComponent* drag = entityManager->getComponent<DraggableComponent>(ent);
        if (drag && drag->isDraggable)
        {
            return ent;
        }
    }
    return NULL_ENTITY;
}

void SceneController::performMove(EntityID from, EntityID to)
{
    PositionComponent* fromPos = entityManager->getComponent<PositionComponent>(from);
    PositionComponent* toPos = entityManager->getComponent<PositionComponent>(to);
    
    if (!fromPos || !toPos) return;
    
    int tempx = fromPos->x;
    fromPos->x = toPos->x;
    toPos->x = tempx;

    int tempy = fromPos->y;
    fromPos->y = toPos->y;
    toPos->y = tempy;

    board->setEntity(fromPos->x, fromPos->y, from);
    board->setEntity(toPos->x, toPos->y, to);

    SpriteComponent* fromSprite = entityManager->getComponent<SpriteComponent>(from);
    SpriteComponent* toSprite = entityManager->getComponent<SpriteComponent>(to);
    
    if (fromSprite)
    {
        calculateRenderPositionFromCoordinate(fromPos->x, fromPos->y, &fromSprite->targetRect);
    }
    if (toSprite)
    {
        calculateRenderPositionFromCoordinate(toPos->x, toPos->y, &toSprite->targetRect);
    }

    checkPossipleCombosOnBoard();
}

void SceneController::checkPossipleCombosOnBoard()
{
    std::set<EntityID> possibleCombos = MatchLogic::findMatches(*board, *entityManager);

    if (possibleCombos.size() > 0)
    {
        removeComboItems(possibleCombos);
    }
}

void SceneController::collapseBoard()
{
    GravityResult gravityResult = GravityLogic::computeCollapse(*board, *entityManager);
    vector<Animation*> animations;

    for (const auto& move : gravityResult.moves)
    {
        Animation* collapse_anim = new Animation(entityManager, move.entity, MOVE, move.toX, move.toY);
        animations.push_back(collapse_anim);
        
        SDL_Point from = { move.fromX, move.fromY };
        SDL_Point to = { move.toX, move.toY };
        SwapData data = { from, to };
        swaps.push(data);
    }

    if (animations.size() > 0)
    {
        animationController->addAnimation(&animations);
        moveFlag = true;
    }
    else if (gravityResult.needsFill)
    {
        fillFlag = true;
    }
}

void SceneController::fillEmptySlots()
{
    std::vector<FillInfo> fills = GravityLogic::computeFills(*board);

    for (const auto& fill : fills)
    {
        EntityID entity = createGamePiece(fill.x, fill.y, fill.tex, true);
        board->setEntity(fill.x, fill.y, entity);
    }

    checkPossipleCombosOnBoard(); 
}

void SceneController::onMouseMotion(SDL_Event* event)
{
    if ((event->motion.state & SDL_BUTTON_LMASK) && selectedEntity != NULL_ENTITY)
    {
        SpriteComponent* sprite = entityManager->getComponent<SpriteComponent>(selectedEntity);
        if (sprite)
        {
            int new_x = sprite->targetRect.x + event->motion.xrel;
            int new_y = sprite->targetRect.y + event->motion.yrel;
            sprite->targetRect.x = new_x;
            sprite->targetRect.y = new_y;
        }
    }
}

void SceneController::onMouseButtonDown(SDL_Event* event)
{
    SDL_Point p;
    p.x = event->button.x;
    p.y = event->button.y;
    selectedEntity = pickEntity(p);
}

void SceneController::onMouseButtonUp(SDL_Event* event)
{
    if (selectedEntity == NULL_ENTITY) return;
    
    SDL_Point t;
    t.x = event->button.x;
    t.y = event->button.y;

    EntityID targetEntity = pickEntity(t);
    
    PositionComponent* selectedPos = entityManager->getComponent<PositionComponent>(selectedEntity);
    PositionComponent* targetPos = entityManager->getComponent<PositionComponent>(targetEntity);
    
    if (targetEntity == NULL_ENTITY || targetEntity == selectedEntity || !selectedPos || !targetPos ||
        (abs(targetPos->x - selectedPos->x) > 1 || abs(targetPos->y - selectedPos->y) > 1) ||
        MatchLogic::checkIfValidMove(*board, *entityManager, selectedEntity, targetEntity) == false)
    {
        SpriteComponent* sprite = entityManager->getComponent<SpriteComponent>(selectedEntity);
        if (sprite && selectedPos)
        {
            calculateRenderPositionFromCoordinate(selectedPos->x, selectedPos->y, &sprite->targetRect);
        }
        selectedEntity = NULL_ENTITY;
        return;
    }

    performMove(selectedEntity, targetEntity);
}

void SceneController::renderScene()
{   
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);  
    
    SDL_RenderCopyEx(renderer, background, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
    
    uiManager->render();
    
    // Render all entities with SpriteComponent
    auto& sprites = entityManager->getAllComponents<SpriteComponent>();
    for (auto& pair : sprites)
    {
        EntityID entity = pair.first;
        SpriteComponent& sprite = pair.second;
        
        SDL_Texture* texture = ResourceController::getResource(sprite.tex);
        if (texture)
        {
            SDL_RenderCopy(renderer, texture, NULL, &sprite.targetRect);
        }
    }
    
    SDL_RenderPresent(renderer);
}

void SceneController::setAnimationController(AnimationController* ac)
{
    animationController = ac;
}

void SceneController::generateLevel()
{
    // Clear existing board and entities
    board->forEachEntity([this](EntityID entity) {
        entityManager->destroyEntity(entity);
    });
    board->clear();
    
    GAME_TEX obj;
    for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
    {
        for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
        {
            do{
                obj = getRandomObj();
            } while ((i >= 2 && 
                      entityManager->getComponent<MatchableComponent>(board->getEntity(i - 1, y))->color == obj && 
                      entityManager->getComponent<MatchableComponent>(board->getEntity(i - 2, y))->color == obj)
                || (y >= 2 && 
                    entityManager->getComponent<MatchableComponent>(board->getEntity(i, y - 1))->color == obj && 
                    entityManager->getComponent<MatchableComponent>(board->getEntity(i, y - 2))->color == obj));

            EntityID entity = createGamePiece(i, y, obj, true);
            board->setEntity(i, y, entity);
        }
    }
}

void SceneController::removeComboItems(const std::set<EntityID>& matches)
{
    comboItems.clear();
    
    for (auto it = matches.begin(); it != matches.end(); ++it)
    {
        score += 1;
        uiManager->updateScore(score);
        
        Animation* anim = new Animation(entityManager, *it, DESTROY);
        animationController->addAnimation(anim);
        comboItems.insert(*it);
    }
    
    if (comboItems.size() > 0)
    {
        collapseFlag = true;        
    }
}

void SceneController::cleanup()
{
    if (board)
    {
        board->forEachEntity([this](EntityID entity) {
            entityManager->destroyEntity(entity);
        });
        board->clear();
        delete board;
        board = NULL;
    }

    if (uiManager)
    {
        uiManager->cleanup();
        delete uiManager;
        uiManager = nullptr;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gameWindow);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void SceneController::setGameStartTime(Uint32 t){
    gameStartTime = t;
}

bool SceneController::checkIfTimeIsUp()
{
    Uint32  current = SDL_GetTicks();
    Uint32 passed_seconds = (current - gameStartTime) / 1000;
    return (passed_seconds > GAME_DURATION);
}
