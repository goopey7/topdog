// Sam Collier 2023
#pragma once

class Scene
{
  public:
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void draw() = 0;
};
