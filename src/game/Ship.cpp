// Sam Collier 2023
#include "Ship.h"
#include <cmath>
#include <raylib.h>

#include "ClientCommands.h"

void Ship::init(bool playerControlled, const std::string& name, Client* client,
				std::map<Ship*, std::vector<ClientUpdateStatus>>* clientUpdates)
{
	textures.push_back(LoadTexture("res/sprites/shipIdle.png"));
	textures.push_back(LoadTexture("res/sprites/shipActive.png"));
	textures.push_back(LoadTexture("res/sprites/shipIdleTilt.png"));
	textures.push_back(LoadTexture("res/sprites/shipActiveTilt.png"));
	bullets.reserve(20);
	this->playerControlled = playerControlled;
	this->name = name;
	this->client = client;
	this->clientUpdates = clientUpdates;
}

void Ship::handleInput(float dt)
{
	float radians = rotation * (float)M_PI / 180.f;
	rotating = 0;

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
		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
		{
			animationIndex = 3;
		}
		else
		{
			animationIndex = 2;
		}
		animFlip = false;
		rotating = -1;
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
	{
		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
		{
			animationIndex = 3;
		}
		else
		{
			animationIndex = 2;
		}
		animFlip = true;
		rotating = 1;
	}

	if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_W) && !IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_S) &&
		!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_D))
	{
		animationIndex = 0;
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		fire();
		auto us = UpdateStatus(position.x, position.y, velocity.x, velocity.y, rotation, true);
		client->sendToServer(us);
	}
}

void Ship::update(float dt)
{
	if (playerControlled)
	{
		handleInput(dt);

		position.x += velocity.x * dt;
		position.y += velocity.y * dt;
	}
	else
	{
		float radians = rotation * (float)M_PI / 180.f;
		if (clientUpdates->contains(this))
		{
			if (clientUpdates->at(this).size() == 3)
			{
				const ClientUpdateStatus msg0 = clientUpdates->at(this)[2];
				const ClientUpdateStatus msg1 = clientUpdates->at(this)[1];
				const ClientUpdateStatus msg2 = clientUpdates->at(this)[0];

				float v0X = (msg1.posx - msg2.posx) / (msg1.time - msg2.time);
				float v1X = (msg0.posx - msg1.posx) / (msg0.time - msg1.time);
				float aX = (v0X - v1X) / (msg0.time - msg2.time);

				float v0Y = (msg1.posy - msg2.posy) / (msg1.time - msg2.time);
				float v1Y = (msg0.posy - msg1.posy) / (msg0.time - msg1.time);
				float aY = (v0Y - v1Y) / (msg0.time - msg2.time);

				float dt = GetTime() - msg0.time;

				position.x = msg0.posx + v0X * dt + 0.5f * aX * dt * dt;
				position.y = msg0.posy + v0Y * dt + 0.5f * aY * dt * dt;
			}
		}
	}

	rotation += rotating * rotationSpeed * dt;

	calculateAnimation();

	for (Bullet& bullet : bullets)
	{
		bullet.update(dt);
	}

	lastVelocity = velocity;
	lastRotation = rotation;
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
		DrawText(name.c_str(), position.x - MeasureText(name.c_str(), 20) / 2.f, position.y + 50,
				 20, WHITE);
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

void Ship::calculateAnimation()
{
	if (playerControlled)
		return;

	if (lastVelocity.x != velocity.x || lastVelocity.y != velocity.y)
	{
		animationIndex = 1;
	}
	else
	{
		animationIndex = 0;
	}
}
