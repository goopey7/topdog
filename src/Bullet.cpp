// Sam Collier 2023

#include "Bullet.h"
#include <cmath>

Bullet::Bullet(Vector2 pos, float rot) : position(pos), rotation(rot) {}

void Bullet::update(float dt)
{
	float radians = rotation * (float)M_PI / 180.f;
	position.x += sinf(radians) * speed * dt;
	position.y += -cosf(radians) * speed * dt;
}

void Bullet::draw() const
{
	DrawCircleV(position, size, RED);
}
