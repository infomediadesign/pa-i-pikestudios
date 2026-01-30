#include "scorelayer.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <entities/director.h>
#include <raygui.h>
#include <pscore/viewport.h>
#include <layers/mainmenulayer.h>

ScoreLayer::ScoreLayer() : m_filemanager("noahistgay.txt")
{
	HighscoreEntries default_entry = {0, "Default1"};

	highscore.push_back(default_entry);
	}

ScoreLayer::~ScoreLayer()
{
}

void ScoreLayer::on_update(float dt)
{
	
	if ( list_state == AWAITING_INPUT ) {
		update_typing();
	}
}
void ScoreLayer::on_render()
{
	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	float spacing  = 10 * scale;

	Vector2 button_size{50 * scale, 25 * scale};

	Rectangle button_rect{origin.x, origin.y, button_size.x, button_size.y};
	auto next_btn_rect = [&button_rect, spacing]() {
		Rectangle rec{button_rect.x, button_rect.y + button_rect.height + spacing, button_rect.width, button_rect.height};
		button_rect = rec;
		return rec;
	};


	GuiSetStyle(DEFAULT, TEXT_SIZE, 6 * scale);

	if ( GuiButton(button_rect, "Main Menu") ) {
		gApp()->call_later([]() { gApp()->pop_layer<ScoreLayer>(); });
		gApp()->call_later([]() { gApp()->switch_layer<AppLayer, MainMenuLayer>(); });
	}
	draw_score_board();
}
void ScoreLayer::load_highscore(const std::string& filename)
{
	// does the file exist? create it if not. / delete highscore / open txt file and check
	m_filemanager.ensurefileexists(filename);
	if ( highscore.size() > 0 ) 
	{
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
	if ( currentscore < highscore[highscore.size() - 1].score ) {
		return false;
	}
	return true;
}

// Checks if the score qualifies as a new highscore and saves it with the player name if applicable
void ScoreLayer::save_new_highscore(int score)
{
	if ( check_for_new_highscore(score) ) {
		if ( list_state == VIEWING ) {
			list_state = AWAITING_INPUT;
			return;
		} else if ( list_state == INPUT_MADE ) 
		{
			if ( highscore.size() >= 10 ) 
			{
				highscore[highscore.size() - 1].score = score;
				highscore[highscore.size() - 1].name  = player_name_input;
			} 
			else 
			{
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
	if ( IsKeyPressed(KEY_BACKSPACE) && !player_name_input.empty() ) {
		player_name_input.pop_back();
	}
	if ( IsKeyPressed(KEY_ENTER) ) {
		list_state = INPUT_MADE;
		save_new_highscore(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
		save_highscore("noahistgay.txt");
	}
}

void ScoreLayer::set_highscore(std::string name, int score)
{
	HighscoreEntries new_entry;
	new_entry.name = name;
	new_entry.score = score;

	highscore.push_back(new_entry);
}

void ScoreLayer::draw_score_board()
{
	auto& vp = gApp()->viewport();
	GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(RED));

	float y_offset	  = 100.0f;
	float line_height = 30.0f;
	
	

	for ( const auto& entry: highscore ) 
	{
		Rectangle label_rect = {(float) vp->viewport_base_size().x / 2 - 100, y_offset, 200, line_height};


		std::string text = entry.name + " " + std::to_string(entry.score);
		GuiLabel(label_rect, text.c_str());

		y_offset += line_height;
	}
	if ( list_state == AWAITING_INPUT ) {
		Rectangle prompt_label_rect = {(float) vp->viewport_base_size().x / 2 - 100, y_offset - line_height, 200, line_height * highscore.size()};
		std::string prompt_text		= "Enter Name: " + player_name_input;
		std::string your_score_text = "Your Score: " + std::to_string(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
		GuiLabel(prompt_label_rect, your_score_text.c_str());
		prompt_label_rect = {(float) vp->viewport_base_size().x / 2 - 100, y_offset - line_height, 200, line_height * (highscore.size() + 2)};
		GuiLabel(prompt_label_rect, prompt_text.c_str());
	}

	GuiSetStyle(DEFAULT, TEXT_SIZE, 10);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(DARKGRAY));
}
