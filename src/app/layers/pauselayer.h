#pragma once

#include <psinterfaces/layer.h>
#include "entities/director.h"

class PauseLayer: public PSInterfaces::Layer
{
	public:
	PauseLayer();
	
	void on_update(float dt) override;
	void on_render() override;

	void draw_statistics();
	void draw_time(float scale);
	void draw_kill_stats(float scale);
	void draw_player_stats(float scale);
	void init_stat_strings();
	
	enum BtnState {
		Idle = 0,
		Hovered,
		Down
	};

	struct StatInfo{
		std::string name;
		std::string value;
		bool is_upgraded;
	};
	
	private:
	Rectangle m_quit_bounding_rect;
	BtnState m_current_quit_state = Idle;
	Texture2D m_button;

	FortunaDirector* m_director;
	Rectangle m_stats_base_bounds;

	std::vector<StatInfo> m_player_stats;
	std::vector<std::string> m_player_stat_lines;
	std::vector<std::string> m_kill_stat_lines;
};