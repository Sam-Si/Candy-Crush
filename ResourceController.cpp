#include "ResourceController.h"
#include <SDL2/SDL_image.h>
#include <iostream>

// Static member definition - automatically cleaned up on program exit
std::map<std::string, ResourceController::TexturePtr> ResourceController::resourceMap;

ResourceController::ResourceController()
	: renderer(nullptr)
{
}

ResourceController::~ResourceController()
{
	// RAII: resources are automatically cleaned up when resourceMap is destroyed
}

bool ResourceController::init(SDL_Renderer* r)
{
	renderer = r;
	return true;
}

ResourceController::TexturePtr ResourceController::loadTexture(
	const char* path, Uint8 r, Uint8 g, Uint8 b, int w, int h)
{
	SDL_Surface* surface = IMG_Load(path);
	if (surface)
	{
		SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		return TexturePtr(rawTexture);
	}
	
	// Create placeholder texture if file loading fails
	surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
	if (surface)
	{
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
		SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		return TexturePtr(rawTexture);
	}
	
	return TexturePtr(nullptr);
}

bool ResourceController::loadResources()
{
	if (renderer == nullptr)
	{
		std::cout << "Renderer should be initialized via init() method first" << std::endl;
		return false;
	}

	// Clear any existing resources
	resourceMap.clear();

	// Load background
	resourceMap["background"] = loadTexture("./images/BackGround.jpg", 30, 30, 30, 755, 600);

	// Load game objects with string identifiers
	struct { const char* id; Uint8 r, g, b; const char* path; } objects[] = {
		{ "blue", 0, 0, 255, "./images/Blue.png" },
		{ "green", 0, 255, 0, "./images/Green.png" },
		{ "purple", 128, 0, 128, "./images/Purple.png" },
		{ "red", 255, 0, 0, "./images/Red.png" },
		{ "yellow", 255, 255, 0, "./images/Yellow.png" }
	};

	for (const auto& obj : objects)
	{
		resourceMap[obj.id] = loadTexture(obj.path, obj.r, obj.g, obj.b, 38, 38);
	}

	return true;
}

SDL_Texture* ResourceController::getResource(const std::string& textureId)
{
	auto it = resourceMap.find(textureId);
	if (it == resourceMap.end())
	{
		return nullptr;
	}
	return it->second.get();
}
