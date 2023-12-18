// Sam Collier 2023
#pragma once

#include <raylib.h>
class Bullet
{
	public:
		Bullet(Vector2 pos, float rot);
		Bullet(Vector2 pos, Vector2 vel);
		void update(float dt);
		void draw() const;
		Vector2 getPosition() const { return position; }
		float getSize() const { return size; }
	private:
		Vector2 position;
		Vector2 velocity = {0.f, 0.f};
		float speed = 500.f;
		float rotation;
		float size = 4.f;
};
