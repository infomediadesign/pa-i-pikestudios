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

	int peter = 1520;
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt({30, 30, 30, 200}));
	GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt({0, 0, 0, 0}));
	GuiPanel({24, 24, 184, 50}, NULL);
	if ( auto& vp = gApp()->viewport() ) {
		vp->draw_in_viewport(
				kurt, {0, 0, (float) kurt.width, (float) kurt.height}, {(float)kurt.width / 2, (float)kurt.height / 2}, 0.0f,
				WHITE 
		);
		auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
		if ( !director ) {
			return;
		}

		peter			 = director->m_b_bounty.bounty();
		Vector2 text_pos = vp->position_viewport_to_global({120 / 6, 40 / 6});
		GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(RED));
		GuiLabel({text_pos.x, text_pos.y, 72, 24}, std::to_string(peter).c_str());
		GuiSetStyle(DEFAULT, TEXT_SIZE, STANDART_TEXT_SIZE);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(DARKGRAY));
	}
}

void UILayer::draw_bounty_ui(UIBountyContainer container)
{
	calculate_texture_bounds(container.texture, container.texture_bounds, container.bounds, container.padding);
	calculate_text_bounds(container.text, container.text_bounds, container.bounds, container.padding, container.bounds.y);
	container.bounds.width = container.texture_bounds.width + 20;
	GuiPanel(container.bounds, NULL);
	//printf("%f", container.bounds);
	if ( auto& vp = gApp()->viewport() ) {
		vp->draw_in_viewport(
				container.texture, container.texture_bounds, {0, 0}, 0.0f,
				WHITE
		);
	}
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