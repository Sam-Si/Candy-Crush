#pragma once

/*
	ISystem: Base interface for all ECS systems.
	All systems must implement update(dt) to process their domain.
*/
class ISystem
{
public:
	virtual ~ISystem() = default;

	// Process system logic for a frame. dt is delta time in milliseconds.
	virtual void update(float dt) = 0;
};