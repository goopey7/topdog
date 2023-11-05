// Sam Collier 2023
#pragma once

#include <raylib.h>
class Bullet
{
	public:
		Bullet(Vector2 pos, float rot);
		void update(float dt);
		void draw() const;
	private:
		Vector2 position;
		float speed = 500.f;
		float rotation;
		float size = 4.f;
};
