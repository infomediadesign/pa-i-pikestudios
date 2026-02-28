#include "optionslayer.h"
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <pscore/settings.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <raylib.h>

class SettingValues
{
public:
	float music_volume = 20.0f;
	float sfx_volume   = 50.0f;
	bool fullscreen	   = false;
	bool vsync		   = true;
	bool msaa4x		   = true;
	int fps_index	   = 1; // 0:30, 1:60, 2:120, 3:144, 4:240, 5:Unlimited

	int key_accelerate	= KEY_W;
	int key_brake		= KEY_S;
	int key_left_turn	= KEY_A;
	int key_right_turn	= KEY_D;
	int key_left_shoot	= KEY_LEFT;
	int key_right_shoot = KEY_RIGHT;
	int key_all_shoot	= KEY_SPACE;

	bool has_conflicts = false;
};

static const char* FPS_OPTIONS_TEXT = "30;60;120;144;240;Unlimited";

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

	if ( PSCore::SettingsManager::inst()->settings.find("user_preferences") != PSCore::SettingsManager::inst()->settings.end() ) {
		auto& settings				= PSCore::SettingsManager::inst()->settings["user_preferences"];
		m_settings->music_volume	= std::get<float>(settings->value("music_volume").value_or(20.0f));
		m_settings->sfx_volume		= std::get<float>(settings->value("sfx_volume").value_or(50.0f));
		m_settings->fullscreen		= std::get<bool>(settings->value("fullscreen").value_or(true));
		m_settings->vsync			= std::get<bool>(settings->value("vsync").value_or(false));
		m_settings->msaa4x			= std::get<bool>(settings->value("msaa4x").value_or(false));
		m_settings->fps_index		= std::get<int>(settings->value("fps_index").value_or(1));
		m_settings->key_accelerate	= std::get<int>(settings->value("key_accelerate").value_or(KEY_W));
		m_settings->key_brake		= std::get<int>(settings->value("key_brake").value_or(KEY_S));
		m_settings->key_left_turn	= std::get<int>(settings->value("key_left_turn").value_or(KEY_A));
		m_settings->key_right_turn	= std::get<int>(settings->value("key_right_turn").value_or(KEY_D));
		m_settings->key_left_shoot	= std::get<int>(settings->value("key_left_shoot").value_or(KEY_LEFT));
		m_settings->key_right_shoot = std::get<int>(settings->value("key_right_shoot").value_or(KEY_RIGHT));
		m_settings->key_all_shoot	= std::get<int>(settings->value("key_all_shoot").value_or(KEY_SPACE));
	}
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

	check_for_conflicts_();

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

		if ( GuiButtonTexture(
					 m_button, mainmenu_pos, 0, scale, m_settings->has_conflicts ? GRAY : WHITE, GRAY, "Mainmenu"
			 ) ) {
			if ( !m_settings->has_conflicts )
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


		int prev_color = GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL);
		GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x00000000);

		GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x00000000);
		GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x000000FF);
		GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x444444CC);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0x000000FF);
		GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0x111111FF);
		GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0x444444FF);
		GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x111111FF);
		GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0x444444FF);

		{
			BeginScissorMode(m_panelView.x, m_panelView.y, m_panelView.width, m_panelView.height);
			draw_controls_(anchor, scale, textspacing, boxheight);
			EndScissorMode();
		}

		GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, prev_color);

		if ( m_settings->has_conflicts ) {
			float box_w = btn_width * scale;
			float box_h = 20 * scale;
			float box_x = (mainmenu_pos.x - btn_width / 2.0f) * scale;
			float box_y = (mainmenu_pos.y - btn_height / 2.0f) * scale - box_h - 4 * scale;
			Rectangle bounds{box_x, box_y, box_w, box_h};
			DrawRectangleRec(bounds, {255, 165, 0, 255});
			DrawRectangleLinesEx(bounds, 1 * scale, {200, 120, 0, 255});
			int prev_text_color		= GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
			int prev_text_size		= GuiGetStyle(DEFAULT, TEXT_SIZE);
			int prev_text_alignment = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({140, 60, 0, 255}));
			GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);
			GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
			GuiLabel(bounds, "Key conflicts!");
			GuiSetStyle(LABEL, TEXT_ALIGNMENT, prev_text_alignment);
			GuiSetStyle(DEFAULT, TEXT_SIZE, prev_text_size);
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, prev_text_color);
		}
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
				if ( GuiSlider(
							 {anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, NULL, " %", &m_settings->music_volume, 0, 100
					 ) ) {
					gApp()->set_sound_volume(PSCore::Application::SoundType::Music, m_settings->music_volume);
				}
				break;
			}
			case SettingType::SFX: {
				if ( GuiSlider({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, NULL, " %", &m_settings->sfx_volume, 0, 100) ) {
					gApp()->set_sound_volume(PSCore::Application::SoundType::SFX, m_settings->sfx_volume);
				}
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
				int current_index = m_settings->fps_index;
				GuiComboBox({anchor.x + 300 * scale, y, control_width * scale, boxheight * scale}, FPS_OPTIONS_TEXT, &m_settings->fps_index);
				if ( current_index != m_settings->fps_index ) {
					switch ( m_settings->fps_index ) {
						case 0:
							gApp()->set_target_fps(30);
							break;
						case 1:
							gApp()->set_target_fps(60);
							break;
						case 2:
							gApp()->set_target_fps(120);
							break;
						case 3:
							gApp()->set_target_fps(144);
							break;
						case 4:
							gApp()->set_target_fps(240);
							break;
						case 5:
							gApp()->set_target_fps(0);
							break;
						default:
							break;
					}
				}
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


OptionsLayer::~OptionsLayer()
{
	auto& settings = PSCore::SettingsManager::inst()->settings.at("user_preferences");
	settings->set_value("music_volume", m_settings->music_volume);
	settings->set_value("sfx_volume", m_settings->sfx_volume);
	settings->set_value("fullscreen", m_settings->fullscreen);
	settings->set_value("vsync", m_settings->vsync);
	settings->set_value("msaa4x", m_settings->msaa4x);
	settings->set_value("fps_index", m_settings->fps_index);
	settings->set_value("key_accelerate", m_settings->key_accelerate);
	settings->set_value("key_brake", m_settings->key_brake);
	settings->set_value("key_left_turn", m_settings->key_left_turn);
	settings->set_value("key_right_turn", m_settings->key_right_turn);
	settings->set_value("key_left_shoot", m_settings->key_left_shoot);
	settings->set_value("key_right_shoot", m_settings->key_right_shoot);
	settings->set_value("key_all_shoot", m_settings->key_all_shoot);
	settings->safe();
};

void OptionsLayer::check_for_conflicts_()
{
	// Check for keybind conflicts
	std::vector<std::pair<std::string, int>> keybinds = {
			{"Accelerate", m_settings->key_accelerate}, {"Brake", m_settings->key_brake},			{"Left Turn", m_settings->key_left_turn},
			{"Right Turn", m_settings->key_right_turn}, {"Left Shoot", m_settings->key_left_shoot}, {"Right Shoot", m_settings->key_right_shoot},
			{"All Shoot", m_settings->key_all_shoot},
	};

	for ( size_t i = 0; i < keybinds.size(); ++i ) {
		for ( size_t j = i + 1; j < keybinds.size(); ++j ) {
			if ( keybinds[i].second == keybinds[j].second ) {
				m_settings->has_conflicts = true;
				return;
			}
		}
	}

	m_settings->has_conflicts = false;
};
