#include "scorelayer.h"
#include <algorithm>
#include <entities/director.h>
#include <filesystem>
#include <fstream>
#include <layers/mainmenulayer.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <system_error>

ScoreLayer::ScoreLayer() : m_filemanager(m_score_filename)
{
	HighscoreEntries default_entry = {0, "No score yet"};
	highscore.push_back(default_entry);
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

	draw_score_board();

	anchor02 = vp->viewport_origin();
	GuiSetStyle(DEFAULT, TEXT_SIZE, 7 * scale);
	if ( GuiTextBox(
				 Rectangle{anchor02.x + 184 * scale, anchor02.y + 64 * scale, 120 * scale, 16 * scale}, TextBox000Text, 128, TextBox000EditMode
		 ) ) {
	}
	if ( GuiTextBox(
				 Rectangle{anchor02.x + 184 * scale, anchor02.y + 88 * scale, 120 * scale, 16 * scale}, TextBox001Text, 128, TextBox001EditMode
		 ) ) {
	};
	if ( GuiTextBox(Rectangle{anchor02.x + 184 * scale, anchor02.y + 112 * scale, 120 * scale, 16 * scale}, TextBox002Text, 128, TextBox002EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 184 * scale, anchor02.y + 136 * scale, 120 * scale, 16 * scale}, TextBox003Text, 128, TextBox003EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 184 * scale, anchor02.y + 160 * scale, 120 * scale, 16 * scale}, TextBox004Text, 128, TextBox004EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 184 * scale, anchor02.y + 184 * scale, 120 * scale, 16 * scale}, TextBox005Text, 128, TextBox005EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 184 * scale, anchor02.y + 208 * scale, 120 * scale, 16 * scale}, TextBox006Text, 128, TextBox006EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 184 * scale, anchor02.y + 232 * scale, 120 * scale, 16 * scale}, TextBox007Text, 128, TextBox007EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 184 * scale, anchor02.y + 256 * scale, 120 * scale, 16 * scale}, TextBox008Text, 128, TextBox008EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 184 * scale, anchor02.y + 280 * scale, 120 * scale, 16 * scale}, TextBox009Text, 128, TextBox009EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 64 * scale, 120 * scale, 16 * scale}, TextBox012Text, 128, TextBox012EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 88 * scale, 120 * scale, 16 * scale}, TextBox013Text, 128, TextBox013EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 112 * scale, 120 * scale, 16 * scale}, TextBox014Text, 128, TextBox014EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 136 * scale, 120 * scale, 16 * scale}, TextBox015Text, 128, TextBox015EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 160 * scale, 120 * scale, 16 * scale}, TextBox016Text, 128, TextBox016EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 184 * scale, 120 * scale, 16 * scale}, TextBox017Text, 128, TextBox017EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 208 * scale, 120 * scale, 16 * scale}, TextBox018Text, 128, TextBox018EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 232 * scale, 120 * scale, 16 * scale}, TextBox019Text, 128, TextBox019EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 256 * scale, 120 * scale, 16 * scale}, TextBox020Text, 128, TextBox020EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 336 * scale, anchor02.y + 280 * scale, 120 * scale, 16 * scale}, TextBox021Text, 128, TextBox021EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 64 * scale, 16 * scale, 16 * scale}, TextBox045Text, 128, TextBox045EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 88 * scale, 16 * scale, 16 * scale}, TextBox024Text, 128, TextBox024EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 112 * scale, 16 * scale, 16 * scale}, TextBox025Text, 128, TextBox025EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 136 * scale, 16 * scale, 16 * scale}, TextBox026Text, 128, TextBox026EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 160 * scale, 16 * scale, 16 * scale}, TextBox027Text, 128, TextBox027EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 208 * scale, 16 * scale, 16 * scale}, TextBox028Text, 128, TextBox028EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 184 * scale, 16 * scale, 16 * scale}, TextBox029Text, 128, TextBox029EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 232 * scale, 16 * scale, 16 * scale}, TextBox030Text, 128, TextBox030EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 256 * scale, 16 * scale, 16 * scale}, TextBox031Text, 128, TextBox031EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 152 * scale, anchor02.y + 280 * scale, 16 * scale, 16 * scale}, TextBox032Text, 128, TextBox032EditMode) )
	{};
	Button034Pressed = GuiButton(Rectangle{anchor02.x + 20 * scale, anchor02.y + (360 - 44) * scale, 80 * scale, 24 * scale}, "Mainmenu");
	if ( Button034Pressed ) {
		gApp()->call_later([]() { gApp()->switch_layer<ScoreLayer, MainMenuLayer>();});
	}
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 64 * scale, 16 * scale, 16 * scale}, TextBox035Text, 128, TextBox035EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 88 * scale, 16 * scale, 16 * scale}, TextBox036Text, 128, TextBox036EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 112 * scale, 16 * scale, 16 * scale}, TextBox037Text, 128, TextBox037EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 136 * scale, 16 * scale, 16 * scale}, TextBox038Text, 128, TextBox038EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 160 * scale, 16 * scale, 16 * scale}, TextBox039Text, 128, TextBox039EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 232 * scale, 16 * scale, 16 * scale}, TextBox040Text, 128, TextBox040EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 256 * scale, 16 * scale, 16 * scale}, TextBox041Text, 128, TextBox041EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 280 * scale, 16 * scale, 16 * scale}, TextBox042Text, 128, TextBox042EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 184 * scale, 16 * scale, 16 * scale}, TextBox043Text, 128, TextBox043EditMode) )
	{};
	if ( GuiTextBox(Rectangle{anchor02.x + 472 * scale, anchor02.y + 208 * scale, 16 * scale, 16 * scale}, TextBox044Text, 128, TextBox044EditMode) )
	{};
}
void ScoreLayer::load_highscore(const std::string& filename)
{
	// does the file exist? create it if not. / delete highscore / open txt file and check
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
		if ( player_name_input.size() > 0 ) {
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
	auto& vp = gApp()->viewport();
	GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(RED));

	float y_offset	  = 100.0f;
	float line_height = 30.0f;


	for ( const auto& entry: highscore ) {
		Rectangle label_rect = {(float) vp->viewport_base_size().x / 2 - 100, y_offset, 400, line_height};


		std::string text = entry.name + " " + std::to_string(entry.score);
		GuiLabel(label_rect, text.c_str());

		y_offset += line_height;
	}
	if ( list_state == AWAITING_INPUT ) {
		Rectangle prompt_label_rect = {
				(float) vp->viewport_base_size().x / 2 - 100, y_offset - line_height, 400, 200 + line_height * highscore.size()
		};
		std::string prompt_text		= "Enter Name: " + player_name_input;
		std::string your_score_text = "Your Score: " + std::to_string(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
		GuiLabel(prompt_label_rect, your_score_text.c_str());
		prompt_label_rect = {(float) vp->viewport_base_size().x / 2 - 100, y_offset - line_height, 400, 200 + line_height * (highscore.size() + 2)};
		GuiLabel(prompt_label_rect, prompt_text.c_str());
	}

	GuiSetStyle(DEFAULT, TEXT_SIZE, 10);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(DARKGRAY));
}

std::string ScoreLayer::score_filename() const
{
	return m_score_filename;
}
