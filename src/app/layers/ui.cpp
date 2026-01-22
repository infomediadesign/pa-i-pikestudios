#include <layers/ui.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <iostream>

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
	Rectangle rec {GetScreenWidth() /2.0f,GetScreenHeight() /2.0f, 100, 30};
	if (GuiButton(rec, "Open Image")) {
		/* ACTION */std::cout << "clicked" << std::endl;
	}
}

void UILayer::draw_bounty_ui(int bounty_amount)
{
	// Draw the bounty UI element
}
