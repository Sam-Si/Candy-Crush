#pragma once
#include <map>
#include <memory>
#include <string>
#include <SDL2/SDL.h>

/*
	Custom deleter for SDL_Texture that properly calls SDL_DestroyTexture.
	Used with std::unique_ptr for RAII texture management.
*/
struct SDLTextureDeleter
{
	void operator()(SDL_Texture* texture) const
	{
		if (texture)
		{
			SDL_DestroyTexture(texture);
		}
	}
};

/*
	Responsible for the resources and assets related to game.
	Resource controller first loads all the resources to memory and
	all instances use the same texture from memory with referencing them.
	Uses string identifiers for renderer-agnostic lookup.
	Uses RAII for automatic resource cleanup.
*/
class ResourceController
{
private:
	// Type alias for unique_ptr with custom deleter
	using TexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;
	
	// Static map with string keys and unique_ptr values - automatically cleaned up
	static std::map<std::string, TexturePtr> resourceMap;

public:
	ResourceController();
	~ResourceController();

	// Deleted copy/move to prevent accidental copying
	ResourceController(const ResourceController&) = delete;
	ResourceController& operator=(const ResourceController&) = delete;
	ResourceController(ResourceController&&) = delete;
	ResourceController& operator=(ResourceController&&) = delete;

	bool loadResources();		// loads all resources
	static SDL_Texture* getResource(const std::string& textureId);	// gets a specific texture by string id
	bool init(SDL_Renderer* renderer);	// inits the resource controller

private:
	SDL_Renderer* renderer = nullptr;
	
	// Helper to create a texture from file or placeholder
	TexturePtr loadTexture(const char* path, Uint8 r, Uint8 g, Uint8 b, int w, int h);
};

