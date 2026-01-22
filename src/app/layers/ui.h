#pragma once
#include <psinterfaces/layer.h>
#include <raylib.h>

class UILayer : public PSInterfaces::Layer
{

	public:
		UILayer();
		
		void on_update(const float dt) override;
		void on_render() override;

		void draw_bounty_ui(int bounty_amount);
		
	private:
		Texture2D m_ui_bounty_icon;
};
