#include "pauselayer.h"
#include <raylib.h>
#include <pscore/application.h>
#include <pscore/utils.h>

PauseLayer::PauseLayer()
{
}

void PauseLayer::on_render()
{
	Color btn_clr;
	switch ( m_current_quit_state ) {
		case Idle:
			btn_clr = GRAY;
			break;
		case Hovered:
			btn_clr = WHITE;
			break;
		case Down:
			btn_clr = DARKGRAY;
	}
	DrawRectangleRec(m_quit_bounding_rect, btn_clr);
	
	Font f = GetFontDefault();
	PSUtils::DrawTextBoxed(f, "Quit", m_quit_bounding_rect, 24 , 2, false, BLACK);
}

void PauseLayer::on_update(float dt)
{
	float btn_width	 = 100;
	float btn_height = 50;

	float btn_x = (GetScreenWidth() / 2.f) - (btn_width / 2.f);
	float btn_y = (GetScreenHeight() / 2.f) - (btn_height / 2.f);

	m_quit_bounding_rect = {btn_x, btn_y, btn_width, btn_height};

	Vector2 mouse_point = GetMousePosition();
	bool btn_action		= false;
	if ( CheckCollisionPointRec(mouse_point, m_quit_bounding_rect) ) {
		if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) )
			m_current_quit_state = Down;
		else
			m_current_quit_state = Hovered;

		if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT) )
			btn_action = true;
	} else
		m_current_quit_state = Idle;

	if ( btn_action )
		PSCore::Application::get()->stop();
}
