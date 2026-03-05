#pragma once
#include <functional>
#include <psinterfaces/layer.h>
#include <raylib.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class SettingValues;
class OptionsLayer : public PSInterfaces::Layer
{
public:
	OptionsLayer();
	~OptionsLayer();

	void on_update(float dt) override;

	void on_render() override;
	
	enum class SettingType
	{
		Audio,
		Music,
		SFX,
		Graphics,
		Fullscreen,
		VSync,
		MSAA4x,
		FPS,
		Keybinds,
		Accelerate,
		Brake,
		LeftTurn,
		RightTurn,
		LeftShoot,
		RightShoot,
		AllShoot
	};

	void set_exit_btn_function(std::string_view new_title, std::function<void()> fn);

private:
	std::unique_ptr<SettingValues> m_settings;

	Texture2D m_background;
	Texture2D m_button;
	Texture2D m_paper;
	std::vector<std::string> m_row1 = {"Audio",	   "Music",		 "SFX",	  "Graphics",  "Fullscreen", "V-Sync",	   "MSAA 4x",	  "FPS",
									   "Keybinds", "Accelerate", "Brake", "Left turn", "Right turn", "Left shoot", "Right shoot", "All shoot"};

	bool m_captureKey = false;
	SettingType m_captureTarget = SettingType::Keybinds;

	Rectangle m_panelRec;
	Rectangle m_panelContentRec;
	Rectangle m_panelView;
	Vector2 m_panelScroll;
	
	void draw_controls_(const Vector2& anchor, float scale, float textspacing, float boxheight);
	void check_for_conflicts_();
	
	std::function<void()> m_exit_btn_function;
	std::string m_exit_btn_text = "Main Menu";
};
