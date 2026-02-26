#include "optionslayer.h"
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raygui.h>

class SettingValues
{
public:
	float music_volume = 20.0f;
	float sfx_volume   = 50.0f;
	bool fullscreen	   = false;
	bool vsync		   = true;
	bool msaa4x		   = true;
	int fps_index	   = 1; // 0:30, 1:60, 2:120, 3:144, 4:240

	int key_accelerate	= KEY_W;
	int key_brake		= KEY_S;
	int key_left_turn	= KEY_A;
	int key_right_turn	= KEY_D;
	int key_left_shoot	= KEY_Q;
	int key_right_shoot = KEY_E;
	int key_all_shoot	= KEY_SPACE;
};

static const char* FPS_OPTIONS_TEXT = "30;60;120;144;240";

static std::string key_to_label(int key)
{
	if ( key == 0 ) {
		return "...";
	}
	switch ( key ) {
		case KEY_SPACE:
			return "SPACE";
		case KEY_ENTER:
			return "ENTER";
		case KEY_TAB:
			return "TAB";
		case KEY_BACKSPACE:
			return "BACKSPACE";
		case KEY_DELETE:
			return "DELETE";
		case KEY_UP:
			return "UP";
		case KEY_DOWN:
			return "DOWN";
		case KEY_LEFT:
			return "LEFT";
		case KEY_RIGHT:
			return "RIGHT";
		case KEY_LEFT_SHIFT:
		case KEY_RIGHT_SHIFT:
			return "SHIFT";
		case KEY_LEFT_CONTROL:
		case KEY_RIGHT_CONTROL:
			return "CTRL";
		case KEY_LEFT_ALT:
		case KEY_RIGHT_ALT:
			return "ALT";
		case KEY_LEFT_SUPER:
		case KEY_RIGHT_SUPER:
			return "SUPER";
		case KEY_CAPS_LOCK:
			return "CAPS";
		default:
			break;
	}
	if ( key >= 32 && key <= 125 ) {
		char buf[2] = {(char) key, 0};
		return std::string{buf};
	}
	return std::string{TextFormat("%d", key)};
}

OptionsLayer::OptionsLayer()
{
	m_settings = std::make_unique<SettingValues>();

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
	if ( m_captureKey ) {
		int key = GetKeyPressed();
		if ( key != 0 ) {
			if ( key == KEY_ESCAPE ) {
				m_captureKey = false;
				return;
			}

			switch ( m_captureTarget ) {
				case SettingType::Accelerate:
					m_settings->key_accelerate = key;
					break;
				case SettingType::Brake:
					m_settings->key_brake = key;
					break;
				case SettingType::LeftTurn:
					m_settings->key_left_turn = key;
					break;
				case SettingType::RightTurn:
					m_settings->key_right_turn = key;
					break;
				case SettingType::LeftShoot:
					m_settings->key_left_shoot = key;
					break;
				case SettingType::RightShoot:
					m_settings->key_right_shoot = key;
					break;
				case SettingType::AllShoot:
					m_settings->key_all_shoot = key;
					break;
				default:
					break;
			}

			m_captureKey = false;
		}
	}

	m_settings->fullscreen = IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
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
				{anchor.x, anchor.y, (float) scroll_width * scale, screen_size.y * scale}, NULL, m_panelContentRec, &m_panelScroll, &m_panelView
		);
		GuiSetStyle(DEFAULT, BORDER_WIDTH, prevBorder);
		GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, prevWidth);

		DrawTextureEx(m_paper, {anchor.x, anchor.y + m_panelScroll.y}, 0, vp->viewport_scale(), WHITE);


		int prev_color = GuiGetStyle(SLIDER, BASE_COLOR_NORMAL);
		GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, 0x00000000);

		{
			BeginScissorMode(m_panelView.x, m_panelView.y, m_panelView.width, m_panelView.height);
			draw_controls_(anchor, scale, textspacing, boxheight);
			EndScissorMode();
		}

		GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, prev_color);
	}
}

void OptionsLayer::draw_controls_(const Vector2& anchor, float scale, float textspacing, float boxheight)
{
	int row = 0;
	for ( int i = 0; auto& entry: m_row1 ) {
		if ( i == 0 || i == 3 || i == 8 ) {
			if ( i > 0 )
				row++;
			float line_height = anchor.y + m_panelScroll.y + 60 * scale + textspacing * scale * (float) row + boxheight * scale;
			DrawLineEx({anchor.x + 180 * scale, line_height}, {anchor.x + 460 * scale, line_height}, 2 * scale, BLACK);
		}

		float y		= anchor.y + m_panelScroll.y + 60 * scale + textspacing * scale * (float) row;
		float alt_x = anchor.x + (450 - boxheight) * scale;

		int control_width = 150;

		Rectangle text_bounds = {anchor.x + 200 * scale, y, 100 * scale, boxheight * scale};

		GuiLabel(text_bounds, const_cast<char*>(entry.c_str()));

		switch ( static_cast<SettingType>(i) ) {
			case SettingType::Audio:
			case SettingType::Graphics:
			case SettingType::Keybinds: {
				break;
			}
			case SettingType::Music: {
				GuiSlider({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, NULL, " %", &m_settings->music_volume, 0, 100);
				break;
			}
			case SettingType::SFX: {
				GuiSlider({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, NULL, " %", &m_settings->sfx_volume, 0, 100);
				break;
			}
			case SettingType::Fullscreen: {
				if ( GuiCheckBox({alt_x, y, boxheight * scale, boxheight * scale}, "", &m_settings->fullscreen) ) {
					m_settings->fullscreen = gApp()->toggle_fullscreen();
				}
				break;
			}
			case SettingType::VSync: {
				GuiCheckBox({alt_x, y, boxheight * scale, boxheight * scale}, "", &m_settings->vsync);
				break;
			}
			case SettingType::MSAA4x: {
				GuiCheckBox({alt_x, y, boxheight * scale, boxheight * scale}, "", &m_settings->msaa4x);
				break;
			}
			case SettingType::FPS: {
				GuiComboBox({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, FPS_OPTIONS_TEXT, &m_settings->fps_index);
				break;
			}
			case SettingType::Accelerate: {
				std::string label =
						(m_captureKey && m_captureTarget == SettingType::Accelerate) ? "Press key" : key_to_label(m_settings->key_accelerate);
				if ( GuiButton({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, label.c_str()) ) {
					m_captureKey	= true;
					m_captureTarget = SettingType::Accelerate;
				}
				break;
			}
			case SettingType::Brake: {
				std::string label = (m_captureKey && m_captureTarget == SettingType::Brake) ? "Press key" : key_to_label(m_settings->key_brake);
				if ( GuiButton({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, label.c_str()) ) {
					m_captureKey	= true;
					m_captureTarget = SettingType::Brake;
				}
				break;
			}
			case SettingType::LeftTurn: {
				std::string label =
						(m_captureKey && m_captureTarget == SettingType::LeftTurn) ? "Press key" : key_to_label(m_settings->key_left_turn);
				if ( GuiButton({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, label.c_str()) ) {
					m_captureKey	= true;
					m_captureTarget = SettingType::LeftTurn;
				}
				break;
			}
			case SettingType::RightTurn: {
				std::string label =
						(m_captureKey && m_captureTarget == SettingType::RightTurn) ? "Press key" : key_to_label(m_settings->key_right_turn);
				if ( GuiButton({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, label.c_str()) ) {
					m_captureKey	= true;
					m_captureTarget = SettingType::RightTurn;
				}
				break;
			}
			case SettingType::LeftShoot: {
				std::string label =
						(m_captureKey && m_captureTarget == SettingType::LeftShoot) ? "Press key" : key_to_label(m_settings->key_left_shoot);
				if ( GuiButton({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, label.c_str()) ) {
					m_captureKey	= true;
					m_captureTarget = SettingType::LeftShoot;
				}
				break;
			}
			case SettingType::RightShoot: {
				std::string label =
						(m_captureKey && m_captureTarget == SettingType::RightShoot) ? "Press key" : key_to_label(m_settings->key_right_shoot);
				if ( GuiButton({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, label.c_str()) ) {
					m_captureKey	= true;
					m_captureTarget = SettingType::RightShoot;
				}
				break;
			}
			case SettingType::AllShoot: {
				std::string label =
						(m_captureKey && m_captureTarget == SettingType::AllShoot) ? "Press key" : key_to_label(m_settings->key_all_shoot);
				if ( GuiButton({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, label.c_str()) ) {
					m_captureKey	= true;
					m_captureTarget = SettingType::AllShoot;
				}
				break;
			}
		}

		row++;
		i++;
	}
}


OptionsLayer::~OptionsLayer() {};
