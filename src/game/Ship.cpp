// Sam Collier 2023
#include "Ship.h"
#include <algorithm>
#include <cmath>
#include <raylib.h>

#include "ClientCommands.h"

void Ship::init(bool playerControlled, const std::string& name, Client* client,
				std::map<Ship*, std::vector<ClientUpdateVel>>* clientUpdates)
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
	}
}

void Ship::update(float dt)
{
	if (playerControlled)
	{
		handleInput(dt);
	}
	else
	{
		if (clientUpdates->contains(this))
		{
			if (clientUpdates->at(this).size() == 3)
			{
				// 2 most recent messages
				const ClientUpdateVel msg0 = clientUpdates->at(this)[2];
				const ClientUpdateVel msg1 = clientUpdates->at(this)[1];

				// clamp change in velocity so that the ship doesn't seem to teleport when velocity
				// changes (still kinda happens at 500ms+)
				float dvx = msg0.velx - msg1.velx;
				dvx = std::clamp(dvx, -10.f, 10.f);

				float dvy = msg0.vely - msg1.vely;
				dvy = std::clamp(dvy, -10.f, 10.f);

				velocity.x = msg0.velx + dvx;
				velocity.y = msg0.vely + dvy;
			}
		}
	}

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;

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

void Ship::onCollision(const Bullet& other) { takeDamage(20.f); }

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

void Ship::fire(float posx, float posy, float velx, float vely, float time)
{
	if (bullets.size() >= 20)
	{
		bullets.erase(bullets.begin());
	}
	float dt = GetTime() - time;
	float x = posx + velx * BULLET_SPEED * dt;
	float y = posy + vely * BULLET_SPEED * dt;
	Vector2 pos = {x, y};
	Vector2 vel = {velx, vely};
	bullets.emplace_back(pos, vel);
}

void Ship::startRotation(float angle, short direction, float time)
{
	float dt = GetTime() - time;
	rotation = angle + direction * rotationSpeed * dt;
	rotating = direction;
}

void Ship::endRotation(float angle)
{
	rotation = angle;
	rotating = 0;
}

void Ship::takeDamage(float damage)
{
	health -= damage;
	if (health <= 0)
	{
		isAlive = false;
	}
}
