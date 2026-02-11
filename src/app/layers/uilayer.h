#pragma once
#include <psinterfaces/layer.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <iostream>
#include <entities/director.h>

class UILayer : public PSInterfaces::Layer
{


public:

	struct UIBountyContainer
	{
		Rectangle bounds;
		Texture2D texture;
		std::string text;
		Rectangle texture_bounds;
		Rectangle text_bounds;
		int padding;
	};

	UILayer();

	void on_update(const float dt) override;
	void on_render() override;

	void draw_bounty_ui();

	void draw_text(std::string text, Rectangle bounds, int text_size, Color color);
	void draw_panel(Rectangle bounds, Color color, Color border_color);

	void draw_health_ui();

private:
	Texture2D m_ui_bounty_icon;
	Texture2D bounty_coin = LoadTexture("resources/icon/test_coin.png");
	Texture2D m_health_icon = LoadTexture("resources/icon/health_icon.png");
	UIBountyContainer m_ui_bounty_container;
};
