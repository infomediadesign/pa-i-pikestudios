#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <layers/scorelayer.h>
#include <entities/director.h>
#include <raygui.h>
#include <raylib.h>

#include "optionslayer.h"

static const int STANDART_TEXT_SIZE = 10;

MainMenuLayer::MainMenuLayer()
{
	Vector2 frame_grid{1, 1};
	m_button_1 = PRELOAD_TEXTURE("button1", "resources/ui/button_big_1.png", frame_grid)->m_s_texture;
	m_button_2 = PRELOAD_TEXTURE("button2", "resources/ui/button_big_2.png", frame_grid)->m_s_texture;
	m_button_3 = PRELOAD_TEXTURE("button3", "resources/ui/button_big_3.png", frame_grid)->m_s_texture;
	m_button_4 = PRELOAD_TEXTURE("button4", "resources/ui/button_big_4.png", frame_grid)->m_s_texture;
	m_main_menu_background = PRELOAD_TEXTURE("main_menu_bg", "resources/ui/main_menu_background.png", frame_grid)->m_s_texture;
	m_main_menu_title	   = PRELOAD_TEXTURE("main_menu_title", "resources/ui/fortunas_echo_title.png", frame_grid)->m_s_texture;
	m_button			   = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;
}

void MainMenuLayer::on_update(float dt)
{
}

void MainMenuLayer::on_render()
{
	draw_background();

	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	float spacing = 8;

	Vector2 button_size{128 * scale, 40 * scale};
	
	Rectangle button_rect{origin.x + 40 * scale, origin.y + 136 * scale, button_size.x, button_size.y};
	auto next_btn_rect = [&button_rect, spacing]() {
		Rectangle rec{button_rect.x, button_rect.y + button_rect.height + spacing, button_rect.width, button_rect.height};
		button_rect = rec;
		return rec;
	};

		Vector2 button_pos = {
			origin.x / scale + 40.0f + static_cast<float>(m_button_1.width) / 2.0f,
			origin.y / scale + 136.0f + static_cast<float>(m_button_1.height) / 2.0f
	};

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({251, 145, 49, 255}));

		if ( GuiButtonTexture(m_button_1, button_pos, 0, scale, WHITE, GRAY, "Start Game") ) {
		gApp()->call_later([]() {
			if ( auto& director = gApp()->game_director_ref() ) {
				director.reset(new FortunaDirector());
			}
		});
		gApp()->call_later([]() { gApp()->switch_layer<MainMenuLayer, AppLayer>(); });
	}

	button_pos.y += spacing + m_button_1.height;

	if ( GuiButtonTexture(m_button_2, button_pos, 0, scale, WHITE, GRAY, "Options") )
	{
		gApp()->call_later([]() { PS_LOG(LOG_INFO, "Clicked Settings"); });
		gApp()->call_later([](){gApp()->switch_layer<MainMenuLayer, OptionsLayer>(); });
	}

	button_pos.y += spacing + m_button_1.height;
	
	if ( GuiButtonTexture(m_button_3, button_pos, 0, scale, WHITE, GRAY, "Score Board")) {
		gApp()->call_later([]() { PS_LOG(LOG_INFO, "Clicked Leader Board"); });
		gApp()->call_later([]() { gApp()->switch_layer<MainMenuLayer, ScoreLayer>(); });
		gApp()->call_later([]() { 
			auto score_layer = gApp()->get_layer<ScoreLayer>();
			if ( score_layer )
				score_layer->load_highscore(score_layer->score_filename());
		});
	}

	button_pos.y += spacing + m_button_1.height;

	if ( GuiButtonTexture(m_button_4, button_pos, 0, scale, WHITE, GRAY, "Quit") ) {
		gApp()->stop();
	}

	GuiSetStyle(DEFAULT, TEXT_SIZE, STANDART_TEXT_SIZE);
}

void MainMenuLayer::draw_background()
{
	auto& vp = gApp()->viewport();
	float scale = vp->viewport_scale();
	DrawTextureEx(m_main_menu_background, {vp->viewport_origin().x, vp->viewport_origin().y}, 0, scale, WHITE);
	DrawTextureEx(m_main_menu_title, {vp->viewport_origin().x + 30 * scale, vp->viewport_origin().y + 30}, 0, scale, WHITE
	);
}
