#include <layers/uilayer.h>
#define RAYGUI_IMPLEMENTATION
#include <iostream>
#include <raygui.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raylib.h>
#include <entities/director.h>

static const int STANDART_TEXT_SIZE = 10;
static const Color STANDARD_TEXT_COLOR	= DARKGRAY;

UILayer::UILayer()
{
	m_ui_bounty_container.texture = LoadTexture("ressources/icon/test_coin.png");
}

void UILayer::on_update(const float dt)
{
	// Update UI elements here
}


void UILayer::on_render()
{
	draw_bounty_ui();
}


void UILayer::draw_text(std::string text, Rectangle bounds, int text_size, Color color )
{
	GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(color));
	GuiLabel(bounds, text.c_str());
	GuiSetStyle(DEFAULT, TEXT_SIZE, STANDART_TEXT_SIZE);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(STANDARD_TEXT_COLOR));
}

void UILayer::draw_panel(Rectangle bounds, Color color, Color border_color)
{
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(color));
	GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(border_color));
	GuiPanel(bounds, NULL);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(WHITE));
	GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(WHITE));
}

void UILayer::draw_bounty_ui()
{
	auto& vp = gApp()->viewport();
	Vector2 panel_pos = vp->position_viewport_to_global({0, 0});
	draw_panel({panel_pos.x, panel_pos.y, 184, 55}, {50, 50, 50, 200}, {0, 0, 0, 0});
	vp->draw_in_viewport(
			bounty_coin, {0, 0, (float) bounty_coin.width, (float) bounty_coin.height},
			{(float) bounty_coin.width / 2, (float) bounty_coin.height / 2}, 0.0f, WHITE);


	Vector2 text_pos = vp->position_viewport_to_global({120 / 6, 40 / 6});
	draw_text(
			std::to_string(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty()), {text_pos.x, text_pos.y, 72, 24}, 24,
			RED
		);
	
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
