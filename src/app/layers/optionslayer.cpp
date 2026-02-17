#include "optionslayer.h"
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raygui.h>

OptionsLayer::OptionsLayer()
{
	Vector2 frame_grid{1, 1};
	m_background = PRELOAD_TEXTURE("background", "resources/ui/scoreboard_layer.png", frame_grid)->m_s_texture;
	m_button	 = PRELOAD_TEXTURE("button", "resources/ui/button_small.png", frame_grid)->m_s_texture;
}

void OptionsLayer::on_update(float dt)
{
}

void OptionsLayer::on_render()
{
	if ( auto& vp = gApp()->viewport() ) {
		DrawTextureEx(m_background, vp->viewport_origin(), 0, vp->viewport_scale(), WHITE);

		Vector2 anchor		= vp->viewport_origin();
		float scale			= vp->viewport_scale();
		Vector2 screen_size = vp->viewport_base_size();

		float btn_width				 = static_cast<float>(m_button.width);
		float btn_height			 = static_cast<float>(m_button.height);
		float button_boarder_padding = 20;

		float button_pos_y = screen_size.y - btn_height / 2.0f - button_boarder_padding;

		GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0, 0, 0, 255}));

		Vector2 mainmenu_pos = {anchor.x / scale + button_boarder_padding + btn_width / 2.0f, anchor.y / scale + button_pos_y};

		if ( GuiButtonTexture(m_button, mainmenu_pos, 0, scale, WHITE, GRAY, "Mainmenu") ) {
			gApp()->call_later([]() { gApp()->switch_layer<OptionsLayer, MainMenuLayer>(); });
		}

		float textspacing = 24;
		float boxheight	  = 16;

		for ( int i = 0; auto& entry: m_row1 ) {
			if ( i == 0 || i == 3 || i == 9 ) {
				if ( i > 0 ) {
					i++;
				}
				float line_height = anchor.y + 60 * scale + textspacing * scale * (float) i + boxheight * scale;
				DrawLineEx({anchor.x + 180 * scale, line_height}, {anchor.x + 460 * scale, line_height}, 2 * scale, BLACK);
			}
			Rectangle text_bounds = {anchor.x + 200 * scale, anchor.y + 60 * scale + textspacing * scale * (float) i, 100 * scale, boxheight * scale};

			GuiLabel(text_bounds, const_cast<char*>(entry.c_str()));

			i++;
		}
	}
}
