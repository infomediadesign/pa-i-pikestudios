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
	bounty_coin						= LoadTexture("resources/icon/bounty_icon.png");
	m_health_icon				  = LoadTexture("resources/icon/hp_icon.png");
}

void UILayer::on_update(const float dt)
{
	// Update UI elements here
}


void UILayer::on_render()
{
	draw_bounty_ui();
	draw_health_ui();
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
	auto& vp	= gApp()->viewport();
	float scale				= vp->viewport_scale();
	
	std::string bounty_text = std::to_string(
			dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());

	float texture_width = static_cast<float>(bounty_coin.width);
	float texture_height = static_cast<float>(bounty_coin.height);
	float text_width = bounty_text.length() * 11.0f;
	float padding = 30.0f;

	float panel_padding_horizontal = 3.0f;
	float panel_padding_vertical = 3.0f;
	float texture_padding = 0.0f;
	
	Vector2 panel_size = {texture_width + text_width + padding, texture_height + 2 * panel_padding_vertical};
	Vector2 panel_pos = vp->position_viewport_to_global({0, 0});

	draw_panel({panel_pos.x, panel_pos.y, panel_size.x * scale, panel_size.y * scale }, {0, 0, 0, 50}, {0, 0, 0, 0});
	
	float texture_viewport_x = panel_padding_horizontal + texture_padding + texture_width / 2;
	float texture_viewport_y = panel_padding_vertical + texture_height / 2;
	vp->draw_in_viewport(
			bounty_coin, {0, 0, texture_width, texture_height},
			{texture_viewport_x, texture_viewport_y}, 0.0f, WHITE);

	float text_size	 = 23;
	Vector2 text_pos = vp->position_viewport_to_global({panel_padding_horizontal + texture_padding + texture_width + 10.0f, panel_padding_vertical + (panel_size.y - text_size) / 2}); 
	draw_text(
			bounty_text, {text_pos.x, text_pos.y, (text_width + 36) * scale, text_size * scale}, static_cast<int>(text_size * scale),
			{220, 173, 4, 255}
	);
}

void UILayer::draw_health_ui()
{
	if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
		int health	   = director->player_health();
		int max_health = director->player_max_health();
		auto& vp	   = gApp()->viewport();

		float heart_size = 20.0f;
		float padding	 = 10.0f;

		float vp_height = vp->viewport_base_size().y;

		float y_pos = vp_height - heart_size / 2 - padding;

		for ( int i = 0; i < max_health; ++i ) {
			Color color = i < health ? WHITE : DARKGRAY;
			vp->draw_in_viewport(
					m_health_icon, {0, 0, static_cast<float>(m_health_icon.width), static_cast<float>(m_health_icon.height)},
					{padding + heart_size / 2 + i * (heart_size + 5), y_pos}, 0.0f, color
			);
		}
	}
}
