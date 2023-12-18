// Sam Collier 2023

#include "Bullet.h"
#include <cmath>

Bullet::Bullet(Vector2 pos, float rot) : position(pos), rotation(rot) {}

Bullet::Bullet(Vector2 pos, Vector2 vel) : position(pos), velocity(vel) {}

void Bullet::update(float dt)
{
	if (velocity.x != 0 || velocity.y != 0)
	{
		position.x += velocity.x * speed * dt;
		position.y += velocity.y * speed * dt;
	}
	else
	{
		float radians = rotation * (float)M_PI / 180.f;
		position.x += sinf(radians) * speed * dt;
		position.y += -cosf(radians) * speed * dt;
	}
}

void Bullet::draw() const { DrawCircleV(position, size, RED); }
