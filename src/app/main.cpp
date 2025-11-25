#include <imgui.h>
#include <iostream>
#include <raylib.h>
#include <rlImGui.h>
#include <vector>

#include <PSCore/utils.h>

#include "winglviewporthooks.h"

struct Ball
{
	Vector2 pos;
	Vector2 vel{(float) PSUtils::gen_rand(300, 500), (float) PSUtils::gen_rand(300, 500)};
};

int main(void)
{
	float ballRad = 30;
	int number	  = 1;

	float deltaTime		 = 0.0f;
	float currentTime	 = GetTime();
	float updateDrawTime = 0.0f, previousTime = 0.0f, waitTime = 0.0f;

	int targetFPS	  = 0;
	float timeCounter = 0.0f;

	InitWindow(1440, 780, "Fortunas Echo");
	rlImGuiSetup(true);

	std::vector<Ball> balls;

	auto& io = ImGui::GetIO();
	
#ifdef IMGUI_HAS_DOCK
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

	// if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
	// 	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	// 	IM_ASSERT(platform_io.Renderer_CreateWindow == NULL);
	// 	IM_ASSERT(platform_io.Renderer_DestroyWindow == NULL);
	// 	IM_ASSERT(platform_io.Renderer_SwapBuffers == NULL);
	// 	IM_ASSERT(platform_io.Platform_RenderWindow == NULL);
	// 	platform_io.Renderer_CreateWindow  = GLHooks::Hook_Renderer_CreateWindow;
	// 	platform_io.Renderer_DestroyWindow = GLHooks::Hook_Renderer_DestroyWindow;
	// 	platform_io.Renderer_SwapBuffers   = GLHooks::Hook_Renderer_SwapBuffers;
	// 	platform_io.Platform_RenderWindow  = GLHooks::Hook_Platform_RenderWindow;
	// }
	
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

		rlImGuiBeginDelta(deltaTime);

#ifdef IMGUI_HAS_DOCK
		ImGui::DockSpaceOverViewport(
				0, NULL, ImGuiDockNodeFlags_PassthruCentralNode
		); // set ImGuiDockNodeFlags_PassthruCentralNode so that we can see the raylib contents behind the dockspace
#endif

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

		// // Update and Render additional Platform Windows
		// if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
		// 	ImGui::UpdatePlatformWindows();
		// 	ImGui::RenderPlatformWindowsDefault();
		// 	// TODO for OpenGL: restore current GL context.
		// }

		ImGui::End();
		rlImGuiEnd();

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
