#include "scorelayer.h"
#include <algorithm>
#include <entities/director.h>
#include <filesystem>
#include <fstream>
#include <layers/mainmenulayer.h>
#include <misc/leaderboardclient.h>
#include <pscore/utils.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <raylib.h>
#include <thread>

ScoreLayer::ScoreLayer() : m_filemanager(m_score_filename)
{
	HighscoreEntries default_entry = {0, "No score yet"};
	highscore.push_back(default_entry);
	m_scoreboard_background = LoadTexture("resources/ui/scoreboard_layer.png");

	Vector2 frame_grid{1, 1};
	m_button	   = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;
	m_mark_texture = PRELOAD_TEXTURE("mark", "resources/ui/mark.png", frame_grid)->m_s_texture;

	texture_emissive_pos = GetShaderLocation(m_small_ship_shader, "texture_emissive");
	texture_color_pos	 = GetShaderLocation(m_small_ship_shader, "emissive_color");
}

ScoreLayer::~ScoreLayer()
{
}

void ScoreLayer::on_update(float dt)
{
	m_time_since_lase_input += dt;
	if ( list_state == AWAITING_INPUT ) {
		update_typing();
	}
}

void ScoreLayer::on_render()
{
	if ( m_layer_is_visible ) {
		auto& vp	   = gApp()->viewport();
		Vector2 origin = vp->viewport_origin();
		float scale	   = vp->viewport_scale();

		vp->draw_in_viewport(
				m_scoreboard_background,
				{0, 0, static_cast<float>(m_scoreboard_background.width), static_cast<float>(m_scoreboard_background.height)},
				{vp->viewport_base_size().x / 2, vp->viewport_base_size().y / 2}, 0, WHITE
		);

		draw_score_board();
		draw_score_board_buttons();
	}
}

// does the file exist? create it if not. / delete highscore / open txt file and check
void ScoreLayer::load_highscore(const std::string& filename)
{
	m_filemanager.ensurefileexists(filename);
	if ( highscore.size() > 0 ) {
		highscore.clear();
	}
	std::ifstream infile(filename);
	if ( !infile.is_open() )
		return;

	// create 2 temporary storage locations / iterate through the txt file and write each new score and name as one entry into the list
	std::string name;
	int score;
	int sail_clr;
	while ( infile >> name >> score >> sail_clr ) {
		highscore.push_back({score, name, PSUtils::color_to_vector3(sail_clr)});
	}
	infile.close();

	// sort and display during loading
	std::sort(highscore.begin(), highscore.end(), [](const HighscoreEntries& a, const HighscoreEntries& b) -> bool { return a.score > b.score; });
	highscores_loaded = true;
};
void ScoreLayer::save_highscore(const std::string& filename)
{
	// check whether the file exists and open the file
	m_filemanager.ensurefileexists(filename);
	std::ofstream outfile(filename);
	if ( !outfile.is_open() )
		return;

	// extract the highscore list from the game and insert it into the txt file, then close it
	for ( const HighscoreEntries& entry: highscore ) {
		outfile << entry.name << " " << entry.score << " " << PSUtils::vector3_to_color(entry.sail_color) << "\n";
	}
	outfile.close();
};
// if the achieved high score fits into the top 10 list: true, otherwise false
bool ScoreLayer::check_for_new_highscore(int currentscore)
{
	if ( highscore.empty() ) {
		return true;
	}

	if ( highscore.size() < 10 ) {
		return true;
	}

	return currentscore > highscore.back().score;
}

// Checks if the score qualifies as a new highscore and saves it with the player name if applicable
void ScoreLayer::save_new_highscore(int score)
{
	if ( check_for_new_highscore(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty()) ) {
		if ( list_state == VIEWING ) {
			list_state = AWAITING_INPUT;
			return;
		} else if ( list_state == INPUT_MADE ) {
			const int current_sail_clr =
					std::get<int>(PSCore::SettingsManager::inst()->settings["user_preferences"]->value("player_color").value_or(0));
			if ( highscore.size() >= 10 ) {
				highscore.back().score		= score;
				highscore.back().name		= player_name_input;
				highscore.back().sail_color = PSUtils::color_to_vector3(current_sail_clr);
			} else {
				set_highscore(player_name_input, score, PSUtils::color_to_vector3(current_sail_clr));
			}

			std::thread save_thread([this, score, current_sail_clr]() {
				LeaderboardClient client("http://87.106.28.241:18080");
				client.postScore(player_name_input, score, current_sail_clr);
			});
			save_thread.detach();
		}
		std::sort(highscore.begin(), highscore.end(), [](const HighscoreEntries& a, const HighscoreEntries& b) -> bool { return a.score > b.score; });
	}
}
// processes all pressed keys and uses them for the player name
void ScoreLayer::update_typing()
{
	int key = GetCharPressed();
	while ( key > 0 ) {
		if ( (key >= 33) && (key <= 126) && (player_name_input.length() < max_name_length) ) {
			player_name_input.push_back((char) key);
		}
		key = GetCharPressed();
	}
	if ( IsKeyDown(KEY_BACKSPACE) && !player_name_input.empty() && m_time_since_lase_input > 0.08 ) {
		player_name_input.pop_back();
		m_time_since_lase_input = 0.0f;
	}
	if ( IsKeyPressed(KEY_ENTER) ) {
		if ( player_name_input.size() > 0 ) {
			list_state = INPUT_MADE;
			save_new_highscore(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
			save_highscore(m_score_filename);
		}
	}
}

void ScoreLayer::set_highscore(std::string& name, int score, const Vector3& sail_color)
{
	HighscoreEntries new_entry;
	new_entry.name		 = name;
	new_entry.score		 = score;
	new_entry.sail_color = sail_color;

	highscore.push_back(new_entry);
}

void ScoreLayer::draw_score_board()
{
	auto& vp	   = gApp()->viewport();
	float scale	   = vp->viewport_scale();
	Vector2 anchor = vp->viewport_origin();

	GuiSetStyle(DEFAULT, TEXT_SIZE, 7 * scale);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt({0, 0, 0, 20}));
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0, 0, 0, 255}));
	float halfe_screen_x = vp->viewport_base_size().x / 2;
	Rectangle name_field_rect{anchor.x + 232 * scale, anchor.y + 64 * scale, 80 * scale, 16 * scale};
	Rectangle score_field_rect{
			name_field_rect.x + name_field_rect.width + 16 * scale, name_field_rect.y, name_field_rect.width, name_field_rect.height
	};
	Rectangle left_rank_rect{name_field_rect.x - 32 * scale, name_field_rect.y, 16 * scale, name_field_rect.height};
	Rectangle right_color_rect{score_field_rect.x + score_field_rect.width + 16 * scale, name_field_rect.y, 16 * scale, name_field_rect.height};
	Rectangle mark_rect{left_rank_rect.x - 16 * scale, left_rank_rect.y, (float) m_mark_texture.width, (float) m_mark_texture.height};

	int rank	= 1;
	int spacing = 24 * scale;

	for ( const auto& entry: highscore ) {

		// GuiPanel(name_field_rect, NULL);
		GuiLabel(name_field_rect, (" " + entry.name).c_str());

		// GuiPanel(score_field_rect, NULL);
		GuiLabel(score_field_rect, (" " + std::to_string(entry.score)).c_str());

		// GuiPanel(left_rank_rect, NULL);
		GuiLabel(
				{left_rank_rect.x + 2, left_rank_rect.y, left_rank_rect.width + 2, left_rank_rect.height}, ("" + std::to_string(rank) + ".").c_str()
		);

		SetShaderValue(m_small_ship_shader, texture_color_pos, &entry.sail_color, SHADER_UNIFORM_VEC3);

		BeginShaderMode(m_small_ship_shader);
		SetShaderValueTexture(m_small_ship_shader, texture_color_pos, player_emissive);

		float ship_texture_margins = 11 * scale;
		DrawTexturePro(
				m_small_ship_texture, {0, 0, static_cast<float>(m_small_ship_texture.width), static_cast<float>(m_small_ship_texture.height)},
				{right_color_rect.x - (ship_texture_margins / 2), right_color_rect.y - (ship_texture_margins / 2),
				 right_color_rect.width + ship_texture_margins, right_color_rect.height + ship_texture_margins},
				{0, 0}, 0, WHITE
		);

		EndShaderMode();

		if ( entry.name == gApp()->current_player_name() ) {
			DrawTexturePro(
					m_mark_texture, {0, 0, static_cast<float>(m_mark_texture.width), static_cast<float>(m_mark_texture.height)},
					{left_rank_rect.x - m_mark_texture.width * scale, left_rank_rect.y, m_mark_texture.width * scale, m_mark_texture.height * scale},
					{0, 0}, 0, WHITE
			);
		}

		name_field_rect.y += spacing;
		score_field_rect.y += spacing;
		left_rank_rect.y += spacing;
		right_color_rect.y += spacing;
		mark_rect.y += spacing;
		rank++;
	}
}

void ScoreLayer::draw_score_board_buttons()
{
	auto& vp			= gApp()->viewport();
	Vector2 anchor		= vp->viewport_origin();
	float scale			= vp->viewport_scale();
	Vector2 screen_size = vp->viewport_base_size();

	float btn_width				 = static_cast<float>(m_button.width);
	float btn_height			 = static_cast<float>(m_button.height);
	float button_boarder_padding = 20;

	float button_pos_y = screen_size.y - btn_height / 2.0f - button_boarder_padding;

	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);

	Vector2 mainmenu_pos = {anchor.x / scale + button_boarder_padding + btn_width / 2.0f, anchor.y / scale + button_pos_y};

	if ( GuiButtonTexture(m_button, mainmenu_pos, 0, scale, WHITE, GRAY, "Mainmenu") ) {
		gApp()->call_later([]() { gApp()->switch_layer<ScoreLayer, MainMenuLayer>(); });
		gApp()->play_ui_sound(0);
	}

	if ( m_retry_button_visible ) {
		Vector2 retry_pos = {anchor.x / scale + screen_size.x - button_boarder_padding - btn_width / 2.0f, anchor.y / scale + button_pos_y};

		if ( GuiButtonTexture(m_button, retry_pos, 0, scale, WHITE, GRAY, "Retry") ) {
			gApp()->call_later([]() { gApp()->pop_layer<AppLayer>(); });
			gApp()->call_later([]() { gApp()->switch_layer<ScoreLayer, AppLayer>(); });
			gApp()->call_later([]() { gApp()->game_director_ref().reset(new FortunaDirector()); });
			HideCursor();
			gApp()->play_ui_sound(0);
		}
	}
}

std::string ScoreLayer::score_filename() const
{
	return m_score_filename;
}

void ScoreLayer::set_layer_is_visible(bool visible)
{
	m_layer_is_visible = visible;
}

void ScoreLayer::set_retry_button_visible(bool visible)
{
	m_retry_button_visible = visible;
}

void ScoreLayer::reset_state()
{
	highscore.clear();
	HighscoreEntries default_entry = {0, "No score yet"};
	highscore.push_back(default_entry);
	highscores_loaded = false;
	player_name_input.clear();
	list_state				= VIEWING;
	m_time_since_lase_input = 0.0f;
}
