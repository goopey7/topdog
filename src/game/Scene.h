// Sam Collier 2023
#pragma once

class Scene
{
  public:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene() = default;
	~Scene() = default;

	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void draw() = 0;
};
