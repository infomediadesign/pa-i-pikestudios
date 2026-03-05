#include <entities/director.h>
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <layers/scorelayer.h>
#include <memory>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <raylib.h>

#include "optionslayer.h"
#include "pscore/sprite.h"

static const int STANDART_TEXT_SIZE = 10;


class SkinMenu
{
	friend class MainMenuLayer;

public:
	SkinMenu()
	{
		Vector2 frame_grid{1, 1};
		m_skin_menu = PRELOAD_TEXTURE("menu_skin", "resources/ui/skin_menu.png", frame_grid)->m_s_texture;

		texture_emissive_pos = GetShaderLocation(sail_clr, "texture_emissive");
		texture_color_pos	 = GetShaderLocation(sail_clr, "emissive_color");

		if ( PSCore::SettingsManager::inst()->settings.find("user_preferences") != PSCore::SettingsManager::inst()->settings.end() ) {
			auto& settings = PSCore::SettingsManager::inst()->settings["user_preferences"];
			Vector3 clr =
					PSUtils::color_to_vector3(std::get<int>(settings->value("player_color").value_or(PSUtils::vector3_to_color(m_colors[1][2]))));
			for ( int i = 0; i < 3; ++i ) {
				for ( int j = 0; j < 3; ++j ) {
					if ( m_colors[i][j] == clr ) {
						m_selected_color = {i, j};
						return;
					}
				}
			}
		}
	}
	~SkinMenu() {};

	void draw()
	{
		if ( !m_active )
			return;

		auto& vp		= gApp()->viewport();
		float scale		= vp->viewport_scale();
		Vector2 raw_pos = {177, 185};
		set_pos(vp->position_viewport_to_global(raw_pos));

		DrawTextureEx(m_skin_menu, m_position, 0, scale, WHITE);

		Vector2 origin = m_position;
		Vector2 color_blob_size{16 * scale, 16 * scale};

		// draw a 3x3 grid of color blobs
		Vector2 grid_origin = {origin.x + (8 * scale), origin.y + (8 * scale)};
		for ( int i = 0; i < 3; ++i ) {
			for ( int j = 0; j < 3; ++j ) {
				Vector2 blob_pos = {grid_origin.x + i * (color_blob_size.x + (8 * scale)), grid_origin.y + j * (color_blob_size.y + (8 * scale))};

				Vector2 btn_pos = {blob_pos.x - (2 * scale), blob_pos.y - (2 * scale)};

				bool hovered = CheckCollisionPointRec(GetMousePosition(), Rectangle{blob_pos.x, blob_pos.y, color_blob_size.x, color_blob_size.y});
				if ( hovered ) {
					m_btn_state = Hovered;
					if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) ) {
						m_btn_state = Pressed;
					}
				} else if ( m_selected_color.first == i && m_selected_color.second == j ) {
					m_btn_state = Active;
				} else {
					m_btn_state = Normal;
				}

				Color btn_color;
				switch ( m_btn_state ) {
					case Normal:
						btn_color = Color{97, 10, 0, 255};
						break;
					case Hovered:
						btn_color = Color{153, 15, 0, 255};
						break;
					case Pressed:
						btn_color = Color{255, 0, 0, 255};
						break;
					case Active:
						btn_color = Color{251, 145, 49, 255};
						break;
				}

				DrawRectangleV(btn_pos, {color_blob_size.x + (4 * scale), color_blob_size.y + (4 * scale)}, btn_color); // button background

				Color blob = {
						static_cast<unsigned char>(m_colors[i][j].x), static_cast<unsigned char>(m_colors[i][j].y),
						static_cast<unsigned char>(m_colors[i][j].z), 255
				};
				DrawRectangleV(blob_pos, color_blob_size, blob); // red blobs for demonstration

				if ( hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
					m_selected_color = {i, j};
					if ( PSCore::SettingsManager::inst()->settings.find("user_preferences") != PSCore::SettingsManager::inst()->settings.end() ) {
						auto& settings = PSCore::SettingsManager::inst()->settings["user_preferences"];
						settings->set_value("player_color", PSUtils::vector3_to_color(m_colors[i][j]));
						settings->safe();
					}
				}
			}
		}

		// Draw hull
		float x = raw_pos.x + ((float) m_skin_menu.width / 2);
		vp->draw_in_viewport(player->m_s_texture, player->frame_rect({0, 0}), {x, raw_pos.y + 92}, 90, WHITE);

		SetShaderValue(sail_clr, texture_color_pos, &m_colors[m_selected_color.first][m_selected_color.second], SHADER_UNIFORM_VEC3);

		BeginShaderMode(sail_clr);
		SetShaderValueTexture(sail_clr, texture_color_pos, player_emissive);

		// Draw sails
		vp->draw_in_viewport(player->m_s_texture, player->frame_rect({2, 2}), {x, raw_pos.y + 92}, 90, WHITE);

		EndShaderMode();
	}


	void set_pos(Vector2 pos)
	{
		m_position = pos;
	}

	void toggle_active()
	{
		m_active = !m_active;
	}

	enum BtnState { Normal, Hovered, Pressed, Active };

private:
	Texture2D m_skin_menu;
	Vector2 m_position;
	bool m_active		 = false;
	BtnState m_btn_state = Normal;
	std::pair<int, int> m_selected_color{0, 0};

	Vector3 m_colors[3][3] = {
			{{193, 18, 31}, {247, 127, 0}, {255, 195, 0}},
			{{46, 204, 113}, {0, 158, 96}, {255, 255, 255}},
			{{58, 12, 163}, {157, 78, 221}, {255, 0, 110}}
	};

	Vector2 frame_grid{10, 5};
	std::shared_ptr<PSCore::sprites::Sprite> player = PRELOAD_TEXTURE("player", "resources/entity/player.png", frame_grid);
	Texture2D player_emissive						= LoadTexture("resources/emissive/player_emissive.png");

	Shader sail_clr = LoadShader(NULL, "resources/shader/emissive_color.fs");
	int texture_color_pos;
	int texture_emissive_pos;
};


MainMenuLayer::MainMenuLayer()
{
	Vector2 frame_grid{1, 1};
	m_button_1				 = PRELOAD_TEXTURE("button1", "resources/ui/button_big_1.png", frame_grid)->m_s_texture;
	m_button_2				 = PRELOAD_TEXTURE("button2", "resources/ui/button_big_2.png", frame_grid)->m_s_texture;
	m_button_3				 = PRELOAD_TEXTURE("button3", "resources/ui/button_big_3.png", frame_grid)->m_s_texture;
	m_button_4				 = PRELOAD_TEXTURE("button4", "resources/ui/button_big_4.png", frame_grid)->m_s_texture;
	m_main_menu_background	 = PRELOAD_TEXTURE("main_menu_bg", "resources/ui/main_menu_background.png", frame_grid)->m_s_texture;
	m_main_menu_title		 = PRELOAD_TEXTURE("main_menu_title", "resources/ui/fortunas_echo_title.png", frame_grid)->m_s_texture;
	m_button				 = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;
	m_skin_btn				 = PRELOAD_TEXTURE("skin_btn", "resources/ui/skin_button.png", frame_grid)->m_s_texture;
	m_skin_btn_ship_emissive = PRELOAD_TEXTURE("skin_btn_ship_emissive", "resources/emissive/skin_button_emissive.png", frame_grid)->m_s_texture;
	m_options_btn			 = PRELOAD_TEXTURE("options_btn", "resources/ui/option_button.png", frame_grid)->m_s_texture;

	m_itch_io = PRELOAD_TEXTURE("itchio", "resources/icon/itchio.png", frame_grid)->m_s_texture;

	m_custom_font	 = LoadFontEx("resources/fonts/fax_font.ttf", 126, nullptr, 0);
	SetTextureFilter(m_custom_font.texture, TEXTURE_FILTER_BILINEAR);

	m_skin_menu = std::make_unique<SkinMenu>();
}

void MainMenuLayer::on_update(float dt)
{
}

void MainMenuLayer::on_render()
{
	GuiSetFont(m_custom_font);
	draw_background();

	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	float spacing  = 8;

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
		HideCursor();
		gApp()->play_ui_sound(0);
	}

	button_pos.y += spacing + m_button_1.height;

	if ( GuiButtonTexture(m_options_btn, {button_pos.x - 24, button_pos.y}, 0, scale, WHITE, GRAY, "Options") ) {
		gApp()->call_later([]() { PS_LOG(LOG_INFO, "Clicked Settings"); });
		gApp()->call_later([]() { gApp()->switch_layer<MainMenuLayer, OptionsLayer>(); });
		gApp()->play_ui_sound(0);
	}

	SetShaderValue(
			m_skin_menu->sail_clr, m_skin_menu->texture_color_pos,
			&m_skin_menu->m_colors[m_skin_menu->m_selected_color.first][m_skin_menu->m_selected_color.second], SHADER_UNIFORM_VEC3
	);
	BeginShaderMode(m_skin_menu->sail_clr);
	SetShaderValueTexture(m_skin_menu->sail_clr, m_skin_menu->texture_emissive_pos, m_skin_btn_ship_emissive);

	int skin_clicked = GuiButtonTexture(m_skin_btn, {button_pos.x + 44, button_pos.y}, 0, scale, WHITE, GRAY, "");

	EndShaderMode();

	if ( skin_clicked ) {
		m_skin_menu->toggle_active();
	}

	button_pos.y += spacing + m_button_1.height;

	if ( GuiButtonTexture(m_button_3, button_pos, 0, scale, WHITE, GRAY, "Score Board") ) {
		gApp()->call_later([]() { PS_LOG(LOG_INFO, "Clicked Leader Board"); });
		gApp()->call_later([]() { gApp()->switch_layer<MainMenuLayer, ScoreLayer>(); });
		gApp()->call_later([]() {
			auto score_layer = gApp()->get_layer<ScoreLayer>();
			if ( score_layer )
				score_layer->load_highscore(score_layer->score_filename());
		});
		gApp()->play_ui_sound(0);
	}

	button_pos.y += spacing + m_button_1.height;

	if ( GuiButtonTexture(m_button_4, button_pos, 0, scale, WHITE, GRAY, "Quit") ) {
		gApp()->stop();
	}

	GuiSetStyle(DEFAULT, TEXT_SIZE, STANDART_TEXT_SIZE);

	// Vector2 vp_max =vp->viewport_base_size();

	// Vector2 mini_btn_size = {12 * scale, 12 * scale};
	// int margin			  = 10 * scale;
	// if ( GuiButtonTexture(m_itch_io, Vector2{vp_max.x - mini_btn_size.x - margin, vp_max.y - mini_btn_size.x - margin},0, scale, WHITE, GRAY, "") )
	// {
	// }

	m_skin_menu->draw();
}

void MainMenuLayer::draw_background()
{
	auto& vp	= gApp()->viewport();
	float scale = vp->viewport_scale();
	DrawTextureEx(m_main_menu_background, {vp->viewport_origin().x, vp->viewport_origin().y}, 0, scale, WHITE);
	DrawTextureEx(m_main_menu_title, {vp->viewport_origin().x + 30 * scale, vp->viewport_origin().y + 30}, 0, scale, WHITE);
}

MainMenuLayer::~MainMenuLayer() {};
