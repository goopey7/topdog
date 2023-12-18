// Sam Collier 2023
#pragma once

#include "Bullet.h"
#include <vector>
#include <raylib.h>
#include <cstdint>
#include <string>
#include <map>
#include "Client.h"

class Ship
{
	public:
		void init(bool playerControlled, const std::string& name, Client* client, std::map<Ship*, std::vector<ClientUpdateStatus>>* clientUpdates);
		void update(float dt);
		void draw();
		void onCollision(const Bullet& other);
		std::vector<Bullet>& getBullets() { return bullets; }
		Rectangle getCollisionRect() const;
		const std::string& getName() const { return name; }

		const Vector2& getPosition() const { return position; }
		const Vector2& getVelocity() const { return velocity; }
		float getRotation() const { return rotation; }

		void setPosition(const Vector2& position) { this->position = position; }
		void setVelocity(const Vector2& velocity) { this->velocity = velocity; }
		void setRotation(float rotation) { this->rotation = rotation; }

		void calculateAnimation();
		void fire();
		void fire(float posx, float posy, float velx, float vely, float time);

		void setRotationDirection(short direction) { rotating = direction; }
		short getRotationDirection() const { return rotating; }
	private:
		Vector2 position = { 320, 100 };
		Vector2 velocity = { 0, 0 };
		float rotation = 12;
		float rotationSpeed = 200.f;
		float acceleration = 100.f;
		std::vector<Bullet> bullets;
		bool playerControlled;
		bool isAlive = true;
		short rotating = 0;

		std::vector<Texture2D> textures;

		uint32_t animationIndex = 0;
		bool animFlip = false;

		void handleInput(float dt);

		std::string name;

		Vector2 lastVelocity = { 0, 0 };
		float lastRotation = 0;

		Client* client;
		std::map<Ship*, std::vector<ClientUpdateStatus>>* clientUpdates;
};
