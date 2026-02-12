#pragma once

#include <psinterfaces/layer.h>

class MainMenuLayer : public PSInterfaces::Layer
{
public:
	MainMenuLayer();


		void on_update(float dt) override;
		
		void on_render() override;

		void draw_background();

private:
		Texture2D m_button_1;
		Texture2D m_button_2;
		Texture2D m_button_3;
		Texture2D m_button_4;
		Texture2D m_main_menu_background;
		Texture2D m_main_menu_title;
		Texture2D m_button;
};