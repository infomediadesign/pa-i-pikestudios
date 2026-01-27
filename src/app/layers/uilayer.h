#pragma once
#include <psinterfaces/layer.h>
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

	void draw_bounty_ui(UIBountyContainer container);

	void calculate_texture_bounds(Texture2D& texture, Rectangle& texture_bounds, Rectangle& bounds, int& padding);
	void calculate_text_bounds(std::string& text, Rectangle& text_bounds, Rectangle& bounds, int padding, int bound_height);

private:
	Texture2D m_ui_bounty_icon;
	Texture2D kurt = LoadTexture("ressources/icon/test_coin.png");
	UIBountyContainer m_ui_bounty_container;
	bool m_ui_do_once = false;
	std::shared_ptr<FortunaDirector> m_ui_director;
};
