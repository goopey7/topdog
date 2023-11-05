// Sam Collier 2023
#pragma once

#include <raylib.h>
class Ship
{
	public:
		Ship();
		void update(float dt);
		void draw();
	private:
		Vector2 position = { 320, 100 };
		Vector2 velocity = { 0, 0 };
		float rotation = 0;
		float rotationSpeed = 200.f;
		float acceleration = 100.f;
		Texture2D texture;
};
