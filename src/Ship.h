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
		Texture2D texture;
};
