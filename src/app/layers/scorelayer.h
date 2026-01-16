//
// Created by mariu on 09.01.2026.
//

#pragma once

#include <vector>


#include <pscore/filemanager.h>
#include <psinterfaces/layer.h>

struct HighscoreEntries
{
	int score;
	std::string name;
};

class ScoreLayer : public PSInterfaces::Layer
{
	bool highscores_loaded = false;
	std::vector<HighscoreEntries> highscore;
	void on_update(float dt) override;
	void on_render() override;
	void load_highscore(const std::string & filename = "noahistgay.txt");
	void save_highscore(const std::string & filename = "noahistgay.txt");
	bool check_for_new_highscore(int);
	void ensurefileexists(std::string filename);
	void save_new_highscore(int);
	std::string player_name_input;
	const int max_name_length = 15;
	void update_typing();
	enum liststate { VIEWING, AWAITING_INPUT, TYPING_NAME, INPUT_MADE };
	liststate list_state = VIEWING;
};



