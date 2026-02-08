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
public:
	ScoreLayer();

	~ScoreLayer();

	bool highscores_loaded = false;
	std::vector<HighscoreEntries> highscore;
	void on_update(float dt) override;
	void on_render() override;
	void load_highscore(const std::string& filename = m_score_filename);
	void save_highscore(const std::string& filename = m_score_filename);
	bool check_for_new_highscore(int);
	void save_new_highscore(int);
	std::string player_name_input;
	const int max_name_length = 15;
	void update_typing();
	enum liststate { VIEWING, AWAITING_INPUT, TYPING_NAME, INPUT_MADE };
	liststate list_state = VIEWING;
	void set_highscore(std::string name, int score);
	void draw_score_board();
	std::string score_filename() const;
	

	Filemanager m_filemanager;

	private:
	static inline const std::string m_score_filename = "fortunascore.txt";
	float m_time_since_lase_input				 = 0.0f;

};
