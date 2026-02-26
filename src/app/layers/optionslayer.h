#pragma once
#include <psinterfaces/layer.h>
#include <raylib.h>

class OptionsLayer : public PSInterfaces::Layer
{
public:
	OptionsLayer();

	void on_update(float dt) override;

	void on_render() override;

private:
	Texture2D m_background;
	Texture2D m_button;
	Texture2D m_paper;
	std::vector<std::string> m_row1 = {"Audio",	   "Music",		 "SFX",	  "Graphics",  "Fullscreen", "V-Sync",	   "MSAA 4x",	  "FPS",
									   "Keybinds", "Accelerate", "Brake", "Left turn", "Right turn", "Left shoot", "Right shoot", "All shoot"};

	Rectangle m_panelRec;
	Rectangle m_panelContentRec;
	Rectangle m_panelView;
	Vector2 m_panelScroll;
};
