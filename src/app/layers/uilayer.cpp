#include <layers/uilayer.h>
#define RAYGUI_IMPLEMENTATION
#include <iostream>
#include <raygui.h>

UILayer::UILayer()
{
}

void UILayer::on_update(const float dt)
{
	// Update UI elements here
}

void UILayer::on_render()
{
	// Render UI elements here
	Rectangle rec{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 100, 30};
	if ( GuiButton(rec, "Open Image") ) {
		/* ACTION */ std::cout << "clicked" << std::endl;
	}
}

void UILayer::draw_bounty_ui(UIBountyContainer container)
{
	//m_ui_bounty_container.icon.height =
}

void UILayer::calculate_texture_bounds(Texture2D& texture, Rectangle& texture_bounds, Rectangle& bounds, int& padding)
{
	Vector2 texture_size = {static_cast<float>(bounds.width) - padding *2 , static_cast<float>(bounds.height) - padding *2};
	texture_bounds		 = {bounds.x + padding, bounds.y + padding, texture_size.x, texture_size.y};
}

void UILayer::calculate_text_bounds(std::string& text, Rectangle& text_bounds, Rectangle& bounds, int padding, int bound_height)
{
	float text_x = bounds.x + (bounds.width + text.length() * 5);
	float text_y = bound_height + padding;
	text_bounds	 = {text_x, text_y, bounds.width - padding * 2, bounds.height - padding * 2};
}
/*
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Structure to hold container data
typedef struct UIContainer
{
	Rectangle bounds;
	Texture2D texture;
	const char* text;
	Rectangle textureBounds;
	Rectangle textBounds;
} UIContainer;

// Initialize a UI container
UIContainer InitUIContainer(Rectangle bounds, Texture2D texture, const char* text)
{
	UIContainer container = {0};
	container.bounds	  = bounds;
	container.texture	  = texture;
	container.text		  = text;

	// Calculate texture bounds (left side of container)
	float textureSize		= bounds.height - 20; // padding
	container.textureBounds = (Rectangle) {bounds.x + 10, bounds.y + 10, textureSize, textureSize};

	// Calculate text bounds (right side of container)
	container.textBounds = (Rectangle) {bounds.x + textureSize + 20, bounds.y + 10, bounds.width - textureSize - 30, bounds.height - 20};

	return container;
}

// Draw the UI container
void DrawUIContainer(UIContainer container)
{
	// Draw container background panel
	GuiPanel(container.bounds, NULL);

	// Draw texture
	if ( container.texture.id > 0 ) {
		DrawTexturePro(
				container.texture, (Rectangle) {0, 0, (float) container.texture.width, (float) container.texture.height}, container.textureBounds,
				(Vector2) {0, 0}, 0.0f, WHITE
		);
	}

	// Draw text label
	GuiLabel(container.textBounds, container.text);
}

int main()
{
	InitWindow(800, 600, "raygui UI Container Example");
	SetTargetFPS(60);

	// Load a texture
	Texture2D icon = LoadTexture("icon.png"); // Replace with your texture path

	// Create containers
	UIContainer container1 = InitUIContainer((Rectangle) {50, 50, 400, 100}, icon, "Container with texture and text");

	UIContainer container2 = InitUIContainer((Rectangle) {50, 170, 400, 100}, icon, "Another container example");

	while ( !WindowShouldClose() ) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		// Draw containers
		DrawUIContainer(container1);
		DrawUIContainer(container2);

		EndDrawing();
	}

	UnloadTexture(icon);
	CloseWindow();

	return 0;
}
*/
