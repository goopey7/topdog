// Sam Collier 2023

#include "Ship.h"
#include <cmath>
#include <raylib.h>

Ship::Ship() { texture = LoadTexture("res/sprites/shipIdle.png"); }

void Ship::update(float dt)
{
	float radians = rotation * (float)M_PI / 180.f;
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
	{
		rotation -= rotationSpeed * dt;
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
	{
		rotation += rotationSpeed * dt;
	}
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
	{
		velocity.x += sin(radians) * acceleration * dt;
		velocity.y -= cos(radians) * acceleration * dt;
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
	{
		velocity.x -= sin(radians) * acceleration * dt;
		velocity.y += cos(radians) * acceleration * dt;
	}

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
}

void Ship::draw()
{
	DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height},
				   {position.x, position.y, (float)texture.width, (float)texture.height},
				   {(float)texture.width / 2, (float)texture.height / 2}, rotation, WHITE);
}
