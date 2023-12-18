// Sam Collier 2023
#pragma once

#include <raylib.h>
#define BULLET_SPEED 500.f
class Bullet
{
  public:
	Bullet(Vector2 pos, float rot);
	Bullet(Vector2 pos, Vector2 vel);
	void update(float dt);
	void draw() const;
	Vector2 getPosition() const { return position; }
	float getSize() const { return size; }
	float getSpeed() const { return speed; }

  private:
	Vector2 position;
	Vector2 velocity = {0.f, 0.f};
	float speed = BULLET_SPEED;
	float rotation;
	float size = 4.f;
};
