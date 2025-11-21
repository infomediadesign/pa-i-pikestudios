#include <imgui.h>
#include <iostream>
#include <raylib.h>
#include <rlImGui.h>
#include <vector>

#include <random>

float gen_rand()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(300, 500);

	return distr(gen);
}

struct Ball
{
	Vector2 pos;
	Vector2 vel{gen_rand(), gen_rand()};
};

int main(void)
{
	float ballRad = 30;
	int number = 1;

	float deltaTime		 = 0.0f;
	float currentTime	 = GetTime();
	float updateDrawTime = 0.0f, previousTime = 0.0f, waitTime = 0.0f;
	
	int targetFPS	  = 0;
	float timeCounter = 0.0f;

	InitWindow(1440, 780, "Main");
	rlImGuiSetup(true);

	std::vector<Ball> balls;

	while ( !WindowShouldClose() ) {

		timeCounter += deltaTime;

		PollInputEvents();
		BeginDrawing();

		ClearBackground(WHITE);

		for ( auto& ball: balls ) {
			if ( ball.pos.x >= GetScreenWidth() - ballRad / 2 || ball.pos.x <= 0 + ballRad / 2 )
				ball.vel.x *= -1;

			if ( ball.pos.y >= GetScreenHeight() - ballRad / 2 || ball.pos.y <= 0 + ballRad / 2 )
				ball.vel.y *= -1;

			ball.pos.x += ball.vel.x * deltaTime;
			ball.pos.y += ball.vel.y * deltaTime;

			DrawCircleV(ball.pos, ballRad, RED);
		}

#ifdef _DEBUG
		rlImGuiBeginDelta(deltaTime);
		ImGui::Begin("Game Debug");
		ImGui::Text("%s", TextFormat("CURRENT FPS: %i", (int) (1.0f / deltaTime)));
		ImGui::Text("%s", TextFormat("CURRENT BALLS: %i", (int) (balls.size())));

		ImGui::SliderInt("Balls Nuber", &number, 1, 20);
		
		if ( ImGui::Button("Spawn Ball", {100, 20}) ) {
			for ( int i = 0; i < number; i++ ) {
				balls.push_back({20, 20});
				std::cout << "spawned ball" << std::endl;
			}
		}

		ImGui::SliderInt("Frame Rate Limit", &targetFPS, 0, 500);

		ImGui::End();
		rlImGuiEnd();
#endif

		EndDrawing();

		SwapScreenBuffer();

		currentTime	   = GetTime();
		updateDrawTime = currentTime - previousTime;

		if ( targetFPS > 0 ) // We want a fixed frame rate
		{
			waitTime = (1.0f / (float) targetFPS) - updateDrawTime;
			if ( waitTime > 0.0 ) {
				WaitTime((float) waitTime);
				currentTime = GetTime();
				deltaTime	= (float) (currentTime - previousTime);
			}
		} else
			deltaTime = (float) updateDrawTime;

		previousTime = currentTime;
	}

	rlImGuiShutdown();
	CloseWindow();

	return 0;
}
