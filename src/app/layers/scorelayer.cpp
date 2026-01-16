//
// Created by mariu on 09.01.2026.
//

#include "scorelayer.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <system_error>
void ScoreLayer::on_update(float dt)
{
if ( list_state==AWAITING_INPUT ) {
	update_typing();
}
}
void ScoreLayer::on_render()
{

}
void ScoreLayer::load_highscore(const std::string & filename )
{
	// does the file exist? create it if not. / delete highscore / open txt file and check
	ensurefileexists(filename);
	highscore.clear();
	std::ifstream infile(filename);
	if (!infile.is_open()) return;

// create 2 temporary storage locations / iterate through the txt file and write each new score and name as one entry into the list
	std::string name;
	int score;
	while (infile >> name >> score) {
		highscore.push_back({score, name});
	}
	infile.close();

// sort and display during loading
	std::sort(highscore.begin(), highscore.end(),[](const HighscoreEntries& a, const HighscoreEntries& b)->bool {
		return a.score > b.score;
	});
	highscores_loaded = true;
};
void ScoreLayer::save_highscore(const std::string & filename )
{
	// check whether the file exists and open the file
	ensurefileexists(filename);
	std::ofstream outfile(filename);
	if (!outfile.is_open()) return;

// extract the highscore list from the game and insert it into the txt file, then close it
	for (const HighscoreEntries& entry : highscore)
	{
		outfile << entry.name << " " << entry.score << "\n";
	}
	outfile.close();

};
// if the achieved high score fits into the top 10 list: true, otherwise false
bool ScoreLayer::check_for_new_highscore(int currentscore)
{
	if (currentscore < highscore[highscore.size() - 1].score) {
		return false;
	}
	return true;
}
// check whether the txt file exists; if not, create a new one
void ScoreLayer::ensurefileexists(std::string filename)
{
	namespace fs = std::filesystem;
	std::error_code ec;
	fs::path p(filename.c_str());
	if (p.has_parent_path()) {
		fs::create_directory(p.parent_path(), ec);
	}

	if (!fs::exists(p)) {
		std::ofstream f(filename,std::ios::out);
	}
}
// Checks if the score qualifies as a new highscore and saves it with the player name if applicable
void ScoreLayer::save_new_highscore(int score)
{
	if ( check_for_new_highscore(score) ) {
		if (list_state==VIEWING) {
			list_state = AWAITING_INPUT;
			return;
		}
		else if (list_state==INPUT_MADE){
			highscore[highscore.size() - 1].score = score;
		highscore[highscore.size() - 1].name = player_name_input;
		}
		std::sort(highscore.begin(), highscore.end(),[](const HighscoreEntries& a, const HighscoreEntries& b)->bool {
			return a.score > b.score;
		});
	}
}
// processes all pressed keys and uses them for the player name
void ScoreLayer::update_typing()
{
	int key = GetCharPressed();
	while (key > 0) {
		if ((key >= 33) && (key <= 126) && (player_name_input.length() < max_name_length)) {
			player_name_input.push_back((char)key);
		}
		key = GetCharPressed();
	}
	if (IsKeyPressed(KEY_BACKSPACE) && !player_name_input.empty()) {
		player_name_input.pop_back();
	}
	if (IsKeyPressed(KEY_ENTER)) {
		list_state = INPUT_MADE;
	}
}
