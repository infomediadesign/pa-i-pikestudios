#include "pauselayer.h"
#include <raylib.h>
#include <pscore/application.h>
#include <pscore/utils.h>
#include <raygui.h>
#include <layers/mainmenulayer.h>
#include <layers/applayer.h>
#include <pscore/viewport.h>
#include <layers/scorelayer.h>
#include <entities/director.h>

PauseLayer::PauseLayer()
{
}

void PauseLayer::on_render()
{
	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	float spacing  = 10 * scale;
	Color btn_clr;

	Vector2 button_size{100 * scale, 50 * scale};

		Rectangle button_rect{origin.x, origin.y, button_size.x, button_size.y};
	auto next_btn_rect = [&button_rect, spacing]() {
		Rectangle rec{button_rect.x, button_rect.y + button_rect.height + spacing, button_rect.width, button_rect.height};
		button_rect = rec;
		return rec;
	};

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

		GuiSetStyle(DEFAULT, TEXT_SIZE, 6 * scale);

	if ( GuiButton(button_rect, "Leader Board") ) {
    gApp()->call_later([]() { 
        gApp()->switch_layer<PauseLayer, ScoreLayer>(); 
        
        auto score_layer = gApp()->get_layer<ScoreLayer>();
        if ( score_layer ) {
			score_layer->load_highscore(score_layer->score_filename());
			score_layer->save_new_highscore(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());

        }
    });
}
}

void PauseLayer::on_update(const float dt)
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
