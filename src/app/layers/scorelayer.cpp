#include "scorelayer.h"
#include <algorithm>
#include <entities/director.h>
#include <filesystem>
#include <fstream>
#include <layers/mainmenulayer.h>
#include <pscore/viewport.h>
#include <raygui.h>

ScoreLayer::ScoreLayer() : m_filemanager(m_score_filename)
{
	HighscoreEntries default_entry = {0, "No score yet"};
	highscore.push_back(default_entry);
	m_scoreboard_background = LoadTexture("resources/ui/Scoreboard_and_Background_Layer.png");
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
		float spacing  = 10 * scale;
		vp->draw_in_viewport(
				m_scoreboard_background,{0, 0, static_cast<float>(m_scoreboard_background.width), static_cast<float>(m_scoreboard_background.height)},
				{vp->viewport_base_size().x / 2, vp->viewport_base_size().y / 2}, 0, WHITE);

		Vector2 button_size{50 * scale, 25 * scale};

		Rectangle button_rect{origin.x, origin.y, button_size.x, button_size.y};
		auto next_btn_rect = [&button_rect, spacing]() {
			Rectangle rec{button_rect.x, button_rect.y + button_rect.height + spacing, button_rect.width, button_rect.height};
			button_rect = rec;
			return rec;
		};

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
	while ( infile >> name >> score ) {
		highscore.push_back({score, name});
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
		outfile << entry.name << " " << entry.score << "\n";
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
			if ( highscore.size() >= 10 ) {
				highscore.back().score = score;
				highscore.back().name  = player_name_input;
			} else {
				set_highscore(player_name_input, score);
			}
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
		if ( player_name_input.size() > 0 ) 
		{
		list_state = INPUT_MADE;
		save_new_highscore(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
		save_highscore(m_score_filename);
		}
	}
}

void ScoreLayer::set_highscore(std::string name, int score)
{
	HighscoreEntries new_entry;
	new_entry.name	= name;
	new_entry.score = score;

	highscore.push_back(new_entry);
}

void ScoreLayer::draw_score_board()
{
	auto& vp	   = gApp()->viewport();
	float scale	   = vp->viewport_scale();
	Vector2 anchor = vp->viewport_origin();
	
	GuiSetStyle(DEFAULT, TEXT_SIZE, 7 * scale);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt({0, 0, 0, 20}));
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0,0,0,255}));

	Rectangle name_field_rect{anchor.x + 184 * scale, anchor.y + 64 * scale, 120 * scale, 16 * scale};
	Rectangle score_field_rect{name_field_rect.x + 152 * scale, name_field_rect.y, name_field_rect.width, name_field_rect.height};
	Rectangle left_color_rect{name_field_rect.x - 32 * scale, name_field_rect.y, 16 * scale, name_field_rect.height};
	Rectangle right_color_rect{score_field_rect.x + score_field_rect.width + 16 * scale, name_field_rect.y, 16 * scale, name_field_rect.height};

	int rank = 1;
	int spacing = 24 * scale;

	for ( const auto& entry: highscore ) {

		GuiPanel(name_field_rect, NULL);
		GuiLabel(name_field_rect, (" " + entry.name).c_str());

		GuiPanel(score_field_rect, NULL);
		GuiLabel(score_field_rect, (" " + std::to_string(entry.score)).c_str());

		GuiPanel(left_color_rect, NULL);
		GuiLabel(left_color_rect, (" " + std::to_string(rank) + ".").c_str());

		GuiPanel(right_color_rect, NULL);
		GuiLabel(right_color_rect, "");

		name_field_rect.y += spacing;
		score_field_rect.y += spacing;
		left_color_rect.y += spacing;
		right_color_rect.y += spacing;
		rank++;
	}
}

void ScoreLayer::draw_score_board_buttons()
{
	auto& vp	   = gApp()->viewport();
	Vector2 anchor = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	Vector2 button_size{80 * scale, 24 * scale};

	if ( GuiButton(Rectangle{anchor.x + 40 * scale, anchor.y + 312 * scale, button_size.x, button_size.y}, "Mainmenu") ) {
		gApp()->call_later([]() { gApp()->switch_layer<ScoreLayer, MainMenuLayer>(); });
	}
	if ( m_retry_button_visible ) {
		if ( GuiButton(Rectangle{anchor.x + 520 * scale, anchor.y + 312 * scale, button_size.x, button_size.y}, "Retry") ) {
			gApp()->call_later([]() { gApp()->pop_layer<AppLayer>(); });
			gApp()->call_later([]() { gApp()->switch_layer<ScoreLayer, AppLayer>(); });
			gApp()->call_later([]() { gApp()->game_director_ref().reset(new FortunaDirector()); });
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
	list_state = VIEWING;
	m_time_since_lase_input = 0.0f;
}
