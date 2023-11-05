// Sam Collier 2023
#pragma once

#include "Bullet.h"
#include <vector>
#include <raylib.h>
class Ship
{
	public:
		Ship(bool playerControlled);
		void update(float dt);
		void draw();
		void onCollision(const Bullet& other);
		const std::vector<Bullet>& getBullets() const { return bullets; }
		Rectangle getCollisionRect() const;
	private:
		Vector2 position = { 320, 100 };
		Vector2 velocity = { 0, 0 };
		float rotation = 0;
		float rotationSpeed = 200.f;
		float acceleration = 100.f;
		std::vector<Bullet> bullets;
		bool playerControlled;
		bool isAlive = true;

		Texture2D texture;

		void handleInput(float dt);
		void fire();
};
