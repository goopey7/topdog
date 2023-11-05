#include "Ship.h"
#include <raylib.h>
#include <string>

int main()
{
	InitWindow(1280, 720, "TopDog");

	Ship ship(true);
	Ship enemy(false);

	while (!WindowShouldClose())
	{
		enemy.update(GetFrameTime());
		ship.update(GetFrameTime());

		// check collisions
		for (const Bullet& bullet : ship.getBullets())
		{
			if (CheckCollisionCircleRec(bullet.getPosition(), bullet.getSize(),
										enemy.getCollisionRect()))
			{
				enemy.onCollision(bullet);
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);
		// DrawFPS(10, 10);
		//  draw dt
		//  DrawText(std::to_string(GetFrameTime()).c_str(), 10, 30, 20, WHITE);
		enemy.draw();
		ship.draw();
		EndDrawing();
	}

	CloseWindow();
}
