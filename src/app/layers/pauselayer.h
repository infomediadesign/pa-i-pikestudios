#pragma once

#include <psinterfaces/layer.h>
#include <raylib.h>

class PauseLayer: public PSInterfaces::Layer
{
	public:
	PauseLayer();
	
	void on_update(const float dt) override;
	void on_render() override;
	
	enum BtnState {
		Idle = 0,
		Hovered,
		Down
	};
	
	private:
	Rectangle m_quit_bounding_rect;
	BtnState m_current_quit_state = Idle;
};