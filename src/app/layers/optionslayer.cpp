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
	m_background = PRELOAD_TEXTURE("background", "resources/ui/background_layer.png", frame_grid)->m_s_texture;
	m_button	 = PRELOAD_TEXTURE("button", "resources/ui/button_small.png", frame_grid)->m_s_texture;
	m_paper		 = PRELOAD_TEXTURE("paper", "resources/ui/settings_box.png", frame_grid)->m_s_texture;

	// m_panelRec		  = {0, 0, 200, 150};
	m_panelContentRec = {0, 0, 340, 340};
	m_panelView		  = {0};
	m_panelScroll	  = {99, -20};
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

		m_panelContentRec = {0, 0, (float) m_paper.width * scale, (float) m_paper.height * scale};

		int scroll_width  = m_paper.width;
		int scroll_height = m_paper.height;

		int prevWidth = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
		GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 0);

		int prevBorder = GuiGetStyle(DEFAULT, BORDER_WIDTH);
		GuiSetStyle(DEFAULT, BORDER_WIDTH, 0);

		GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x00000000);
		GuiScrollPanel(
				{anchor.x, anchor.y, (float) scroll_width * scale, screen_size.y * scale}, NULL,
				m_panelContentRec, &m_panelScroll, &m_panelView
		);
		GuiSetStyle(DEFAULT, BORDER_WIDTH, prevBorder);
		GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, prevWidth);

		DrawTextureEx(m_paper, {anchor.x, anchor.y + m_panelScroll.y}, 0, vp->viewport_scale(), WHITE);

		BeginScissorMode(m_panelView.x, m_panelView.y, m_panelView.width, m_panelView.height);
		for ( int i = 0; auto& entry: m_row1 ) {
			if ( i == 0 || i == 3 || i == 9 ) {
				if ( i > 0 ) {
					i++;
				}
				float line_height = anchor.y + m_panelScroll.y + 60 * scale + textspacing * scale * (float) i + boxheight * scale;
				DrawLineEx({anchor.x + 180 * scale, line_height}, {anchor.x + 460 * scale, line_height}, 2 * scale, BLACK);
			}
			Rectangle text_bounds = {
					anchor.x + 200 * scale, anchor.y + m_panelScroll.y + 60 * scale + textspacing * scale * (float) i, 100 * scale, boxheight * scale
			};

			GuiLabel(text_bounds, const_cast<char*>(entry.c_str()));

			i++;
		}
		EndScissorMode();
	}
}
