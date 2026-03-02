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
	std::string time_to_string(float time) const;
	void draw_time(Rectangle bounds, Vector2 origin, float scale);
	void draw_kill_stats(Rectangle bounds, Vector2 origin, float scale);
	
	enum BtnState {
		Idle = 0,
		Hovered,
		Down
	};
	
	private:
	Rectangle m_quit_bounding_rect;
	BtnState m_current_quit_state = Idle;
	Texture2D m_button;

	FortunaDirector* m_director;


};