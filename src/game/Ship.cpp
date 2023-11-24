// Sam Collier 2023
#include "Ship.h"
#include <cmath>
#include <raylib.h>

void Ship::init(bool playerControlled)
{
	texture = LoadTexture("res/sprites/shipActive.png");
	bullets.reserve(20);
	this->playerControlled = playerControlled;
}

void Ship::handleInput(float dt)
{
	float radians = rotation * (float)M_PI / 180.f;
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
	{
		// rotate left
		rotation -= rotationSpeed * dt;
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
	{
		// rotate right
		rotation += rotationSpeed * dt;
	}
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
	{
		// accelerate along forward vector
		velocity.x += sinf(radians) * acceleration * dt;
		velocity.y -= cosf(radians) * acceleration * dt;
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
	{
		// decelerate along forward vector
		velocity.x -= sinf(radians) * acceleration * dt;
		velocity.y += cosf(radians) * acceleration * dt;
	}
	if (IsKeyPressed(KEY_SPACE))
	{
		fire();
	}
}

void Ship::update(float dt)
{
	if (playerControlled)
	{
		handleInput(dt);
	}

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;

	for (Bullet& bullet : bullets)
	{
		bullet.update(dt);
	}
}

void Ship::fire()
{
	if (bullets.size() >= 20)
	{
		bullets.erase(bullets.begin());
	}
	bullets.emplace_back(position, rotation);
}

void Ship::onCollision(const Bullet& other) { isAlive = false; }

void Ship::draw()
{
	if (isAlive)
	{
		DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height},
					   {position.x, position.y, (float)texture.width, (float)texture.height},
					   {(float)texture.width / 2, (float)texture.height / 2}, rotation, WHITE);
	}

	for (const Bullet& bullet : bullets)
	{
		bullet.draw();
	}
}

Rectangle Ship::getCollisionRect() const
{
	return {position.x - texture.width / 2.f, position.y - texture.height / 2.f, (float)texture.width,
			(float)texture.height};
}

