#include "ResourceController.h"
#include <SDL2/SDL_image.h>
#include <iostream>

using namespace std;

ResourceController::ResourceController() : renderer(NULL) {}
ResourceController::~ResourceController() {}

map<GAME_TEX, SDL_Texture*>* ResourceController::resourceMap = new map<GAME_TEX, SDL_Texture*>();

bool ResourceController::init(SDL_Renderer* r)
{
    renderer = r;
    return true;
}

static SDL_Texture* createPlaceholderTexture(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, int w, int h) {
    SDL_Surface* surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    if (surface) {
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }
    return NULL;
}

bool ResourceController::loadResources()
{
    if (renderer == NULL)
    {
        cout << "Renderer should be initialized via init() method first" << endl;
        return false;
    }

    // Try loading background
    SDL_Surface* bg = IMG_Load("./images/BackGround.jpg");
    SDL_Texture* bgTex = NULL;
    if (bg) {
        bgTex = SDL_CreateTextureFromSurface(renderer, bg);
        SDL_FreeSurface(bg);
    } else {
        bgTex = createPlaceholderTexture(renderer, 200, 200, 200, GAME_WITDH, GAME_HEIGHT);
    }
    resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(BACKGROUND, bgTex));

    // Try loading Game Over
    SDL_Surface* over = IMG_Load("./images/gameover.png");
    SDL_Texture* overText = NULL;
    if (over) {
        overText = SDL_CreateTextureFromSurface(renderer, over);
        SDL_FreeSurface(over);
    } else {
        overText = createPlaceholderTexture(renderer, 255, 0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT);
    }
    resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(GAMEOVER, overText));

    // Objects
    struct { GAME_TEX tex; Uint8 r, g, b; const char* path; } objects[] = {
        { BLUE_OBJ, 0, 0, 255, "./images/Blue.png" },
        { GREEN_OBJ, 0, 255, 0, "./images/Green.png" },
        { PURPLE_OBJ, 128, 0, 128, "./images/Purple.png" },
        { RED_OBJ, 255, 0, 0, "./images/Red.png" },
        { YELLOW_OBJ, 255, 255, 0, "./images/Yellow.png" }
    };

    for (auto& obj : objects) {
        SDL_Surface* surf = IMG_Load(obj.path);
        SDL_Texture* tex = NULL;
        if (surf) {
            tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
        } else {
            tex = createPlaceholderTexture(renderer, obj.r, obj.g, obj.b, STONE_WIDTH, STONE_HEIGHT);
        }
        resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(obj.tex, tex));
    }

    return true;
}

SDL_Texture* ResourceController::getResource(GAME_TEX obj)
{
    map<GAME_TEX, SDL_Texture*>::iterator it = resourceMap->find(obj);
    if (it == resourceMap->end())
    {
        return NULL;
    }
    return it->second;
}

void ResourceController::cleanup()
{
    if (resourceMap) {
        map<GAME_TEX, SDL_Texture*>::iterator i = resourceMap->begin();
        for (; i != resourceMap->end(); i++)
        {
            SDL_Texture* texture = i->second;
            if (texture) SDL_DestroyTexture(texture);
        }
        resourceMap->clear();
    }
}
