#include "pauselayer.h"
#include <entities/director.h>
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <layers/uilayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <raylib.h>
#include <layers/upgradelayer.h>

PauseLayer::PauseLayer()
{
	Vector2 frame_grid{1, 1};
	m_button = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;
}

void PauseLayer::on_render()
{
	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	float spacing  = 8;

	float button_width = static_cast<float>(m_button.width);
	float button_height = static_cast<float>(m_button.height);
	Vector2 screen_size = vp->viewport_base_size();
	Vector2 button_pos	= {origin.x / scale + screen_size.x / 2.0f, origin.y / scale + screen_size.y / 2.0f - button_height / 2.0f - 10.0f};

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0, 0, 0, 255}));

	DrawRectangle(origin.x, origin.y, GetScreenWidth() * scale, GetScreenHeight() * scale, Color{0, 0, 0, 150});
	
	if ( GuiButtonTexture(m_button, button_pos, 0, scale, WHITE, GRAY, "Resume") ) {
		gApp()->call_later([]() {
			gApp()->pop_layer<PauseLayer>();
			if ( auto app_layer = gApp()->get_layer<AppLayer>() )
				app_layer->resume();
			auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
			if ( director ) {
				director->set_is_active(true);
			}
		});
		gApp()->call_later([]() {
			auto upgrade_layer = gApp()->get_layer<UpgradeLayer>();
			if ( upgrade_layer ) {
				if ( auto app_layer = gApp()->get_layer<AppLayer>() )
					app_layer->suspend();
				upgrade_layer->m_layer_is_visible = true;
			}
		});
	}

	button_pos.y += button_height + 8.0f;
	
	if ( GuiButtonTexture(m_button, button_pos, 0, scale, WHITE, GRAY, "Main Menu") ) {
		gApp()->call_later([]() {
			gApp()->pop_layer<PauseLayer>();
			gApp()->pop_layer<UILayer>();
			gApp()->pop_layer<UpgradeLayer>();
			gApp()->switch_layer<AppLayer, MainMenuLayer>();

		});
	}
}

void PauseLayer::on_update(float dt)
{
}
