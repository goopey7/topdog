// Sam Collier 2023
#include "Ship.h"
#include <cmath>
#include <raylib.h>

void Ship::init(bool playerControlled, const std::string& name)
{
	textures.push_back(LoadTexture("res/sprites/shipIdle.png"));
	textures.push_back(LoadTexture("res/sprites/shipActive.png"));
	textures.push_back(LoadTexture("res/sprites/shipIdleTilt.png"));
	textures.push_back(LoadTexture("res/sprites/shipActiveTilt.png"));
	bullets.reserve(20);
	this->playerControlled = playerControlled;
	this->name = name;
}

void Ship::handleInput(float dt)
{
	float radians = rotation * (float)M_PI / 180.f;

	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
	{
		// accelerate along forward vector
		velocity.x += sinf(radians) * acceleration * dt;
		velocity.y -= cosf(radians) * acceleration * dt;
		animationIndex = 1;
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
	{
		// decelerate along forward vector
		velocity.x -= sinf(radians) * acceleration * dt;
		velocity.y += cosf(radians) * acceleration * dt;

		animationIndex = 1;
	}

	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
	{
		// rotate left
		rotation -= rotationSpeed * dt;
		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
		{
			animationIndex = 3;
		}
		else
		{
			animationIndex = 2;
		}
		animFlip = false;
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
	{
		// rotate right
		rotation += rotationSpeed * dt;
		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
		{
			animationIndex = 3;
		}
		else
		{
			animationIndex = 2;
		}
		animFlip = true;
	}

	if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_W) && !IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_S) &&
		!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_D))
	{
		animationIndex = 0;
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
		DrawTexturePro(
			textures[animationIndex],
			{0, 0, (animFlip ? -1 : 1) * (float)textures[animationIndex].width,
			 (float)textures[animationIndex].height},
			{position.x, position.y, (float)textures[animationIndex].width,
			 (float)textures[animationIndex].height},
			{(float)textures[animationIndex].width / 2, (float)textures[animationIndex].height / 2},
			rotation, WHITE);
		DrawText(name.c_str(), position.x - MeasureText(name.c_str(), 20) / 2.f, position.y + 50, 20,
				 WHITE);
	}

	for (const Bullet& bullet : bullets)
	{
		bullet.draw();
	}
}

Rectangle Ship::getCollisionRect() const
{
	return {position.x - textures[animationIndex].width / 2.f,
			position.y - textures[animationIndex].height / 2.f,
			(float)textures[animationIndex].width, (float)textures[animationIndex].height};
}
