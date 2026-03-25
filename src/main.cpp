#include "engine/Engine.h"

int main(int argc, char* args[])
{
	Engine engine;
	engine.initControllers();
	engine.initGame();	
	engine.startGame();
	return 0;
}