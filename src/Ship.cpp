// Sam Collier 2023

#include "Ship.h"
#include <raylib.h>

Ship::Ship()
{
	texture = LoadTexture("res/sprites/shipIdle.png");
}

void Ship::update(float dt)
{
	if(IsKeyDown(KEY_LEFT))
	{
		position.x -= 100 * dt;
	}
	if(IsKeyDown(KEY_RIGHT))
	{
		position.x += 100 * dt;
	}
	if(IsKeyDown(KEY_UP))
	{
		position.y -= 100 * dt;
	}
	if(IsKeyDown(KEY_DOWN))
	{
		position.y += 100 * dt;
	}
}

void Ship::draw()
{
	DrawTexture(texture, position.x, position.y, WHITE);
}

