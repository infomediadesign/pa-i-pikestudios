#pragma once

#include <psinterfaces/layer.h>
#include <raylib.h>

class SkinMenu;
class MainMenuLayer : public PSInterfaces::Layer
{
public:
	MainMenuLayer();
	~MainMenuLayer();

	void on_update(float dt) override;

	void on_render() override;

	void draw_background();

private:
		Font m_custom_font{};
		Font m_main_menu_font{};
		Texture2D m_button_1;
		Texture2D m_button_2;
		Texture2D m_button_3;
		Texture2D m_button_4;
		Texture2D m_main_menu_background;
		Texture2D m_main_menu_title;
		Texture2D m_button;
		Texture2D m_itch_io;
		Texture2D m_skin_btn;
		Texture2D m_options_btn;
		
		std::unique_ptr<SkinMenu> m_skin_menu;
};