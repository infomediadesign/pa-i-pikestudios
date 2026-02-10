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
    Vector2 anchor02 = { 0, 0 };

    bool TextBox000EditMode = false;
    char TextBox000Text[128] = "Name";
    bool TextBox001EditMode = false;
    char TextBox001Text[128] = "SAMPLE TEXT";
    bool TextBox002EditMode = false;
    char TextBox002Text[128] = "SAMPLE TEXT";
    bool TextBox003EditMode = false;
    char TextBox003Text[128] = "SAMPLE TEXT";
    bool TextBox004EditMode = false;
    char TextBox004Text[128] = "SAMPLE TEXT";
    bool TextBox005EditMode = false;
    char TextBox005Text[128] = "SAMPLE TEXT";
    bool TextBox006EditMode = false;
    char TextBox006Text[128] = "SAMPLE TEXT";
    bool TextBox007EditMode = false;
    char TextBox007Text[128] = "SAMPLE TEXT";
    bool TextBox008EditMode = false;
    char TextBox008Text[128] = "SAMPLE TEXT";
    bool TextBox009EditMode = false;
    char TextBox009Text[128] = "SAMPLE TEXT";
    bool TextBox012EditMode = false;
    char TextBox012Text[128] = "score";
    bool TextBox013EditMode = false;
    char TextBox013Text[128] = "SAMPLE TEXT";
    bool TextBox014EditMode = false;
    char TextBox014Text[128] = "SAMPLE TEXT";
    bool TextBox015EditMode = false;
    char TextBox015Text[128] = "SAMPLE TEXT";
    bool TextBox016EditMode = false;
    char TextBox016Text[128] = "SAMPLE TEXT";
    bool TextBox017EditMode = false;
    char TextBox017Text[128] = "SAMPLE TEXT";
    bool TextBox018EditMode = false;
    char TextBox018Text[128] = "SAMPLE TEXT";
    bool TextBox019EditMode = false;
    char TextBox019Text[128] = "SAMPLE TEXT";
    bool TextBox020EditMode = false;
    char TextBox020Text[128] = "SAMPLE TEXT";
    bool TextBox021EditMode = false;
    char TextBox021Text[128] = "SAMPLE TEXT";
    bool TextBox022EditMode = false;
    char TextBox022Text[128] = "Your score";
    bool TextBox023EditMode = false;
    char TextBox023Text[128] = "Your name";
    bool TextBox024EditMode = false;
    char TextBox024Text[128] = "SAMPLE TEXT";
    bool TextBox025EditMode = false;
    char TextBox025Text[128] = "SAMPLE TEXT";
    bool TextBox026EditMode = false;
    char TextBox026Text[128] = "SAMPLE TEXT";
    bool TextBox027EditMode = false;
    char TextBox027Text[128] = "SAMPLE TEXT";
    bool TextBox028EditMode = false;
    char TextBox028Text[128] = "SAMPLE TEXT";
    bool TextBox029EditMode = false;
    char TextBox029Text[128] = "SAMPLE TEXT";
    bool TextBox030EditMode = false;
    char TextBox030Text[128] = "SAMPLE TEXT";
    bool TextBox031EditMode = false;
    char TextBox031Text[128] = "SAMPLE TEXT";
    bool TextBox032EditMode = false;
    char TextBox032Text[128] = "SAMPLE TEXT";
    bool Button034Pressed = false;
    bool TextBox035EditMode = false;
    char TextBox035Text[128] = "SAMPLE TEXT";
    bool TextBox036EditMode = false;
    char TextBox036Text[128] = "SAMPLE TEXT";
    bool TextBox037EditMode = false;
    char TextBox037Text[128] = "SAMPLE TEXT";
    bool TextBox038EditMode = false;
    char TextBox038Text[128] = "SAMPLE TEXT";
    bool TextBox039EditMode = false;
    char TextBox039Text[128] = "SAMPLE TEXT";
    bool TextBox040EditMode = false;
    char TextBox040Text[128] = "SAMPLE TEXT";
    bool TextBox041EditMode = false;
    char TextBox041Text[128] = "SAMPLE TEXT";
    bool TextBox042EditMode = false;
    char TextBox042Text[128] = "SAMPLE TEXT";
    bool TextBox043EditMode = false;
    char TextBox043Text[128] = "SAMPLE TEXT";
    bool TextBox044EditMode = false;
    char TextBox044Text[128] = "SAMPLE TEXT";
	bool TextBox045EditMode = false;
	char TextBox045Text[128] = "SAMPLE TEXT";
	Filemanager m_filemanager;

	private:
	static inline const std::string m_score_filename = "fortunascore.txt";
	float m_time_since_lase_input				 = 0.0f;

};
