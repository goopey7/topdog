// Sam Collier 2023
#pragma once

#include "Scene.h"
#include <functional>

class Menu : public Scene
{
  public:
	Menu(const Menu&) = delete;
	Menu& operator=(const Menu&) = delete;
	Menu(std::function<void()> gui) : gui(gui) {}

	void init() final;
	void update(float dt) final;
	void draw() final;

  private:
	std::function<void()> gui;
};
