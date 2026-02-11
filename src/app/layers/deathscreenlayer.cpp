//
// Created by rikab on 29/01/2026.
//
#include <entities/director.h>
#include <layers/deathscreenlayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include "applayer.h"

#include "scorelayer.h"

void DeathScreenLayer::on_update(float dt)
{
	if ( IsKeyPressed(KEY_ENTER) && m_score_layer_instance->player_name_input.size() > 0 ) {
		gApp()->set_current_player_name(m_score_layer_instance->player_name_input);
		m_name_entered = true;
	}
}

void DeathScreenLayer::on_render()
{
	auto& vp   = gApp()->viewport();
	Vector2 np = vp->viewport_origin();
	float sk   = vp->viewport_scale();

	Color bg_clr{0, 0, 0, 150};
	DrawRectangle(np.x, np.y, GetScreenWidth() * sk, GetScreenHeight() * sk, bg_clr);

	float w = 300;
	float x = ((vp->viewport_base_size().x) / 2.0f) - (w / 2);
	Rectangle rect{np.x + x * sk, np.y + 48 * sk, w * sk, 40 * sk};

	std::string bounty_text = std::to_string(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
	Rectangle score{np.x + x * sk, np.y + 100 * sk, w * sk, 40 * sk};
	Rectangle score_info_bounds{np.x + x * sk, np.y + 125 * sk, w * sk, 40 * sk};

	int oldColor = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
	int oldSize	 = GuiGetStyle(DEFAULT, TEXT_SIZE);
	int oldAlign = GuiGetStyle(LABEL, TEXT_ALIGNMENT);

	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xff0000ff);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 28 * sk);

	GuiLabel(rect, "Du bist gestorben :(");

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * sk);

	GuiLabel(score, ("Score: " + bounty_text).c_str());
	if ( m_score_should_be_saved ) {
		GuiLabel(score_info_bounds, "Du hast es in die Top 10 geschafft!");

		float input_row_y  = 200;
		float input_height = 30;
		Rectangle name_prompt_rect{np.x + x * sk, np.y + input_row_y * sk, 200 * sk, input_height * sk};
		Rectangle input_field_rect{np.x + (x + 170) * sk, np.y + input_row_y * sk, 120 * sk, input_height * sk};

		GuiLabel(name_prompt_rect, "Gib deinen Namen ein:");

		if ( !m_name_entered ) {
			GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt({0, 0, 0, 50}));
			GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt({0, 0, 0, 0}));
			GuiPanel(input_field_rect, NULL);
			GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(WHITE));
			GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(WHITE));
		}
		GuiLabel(input_field_rect, (m_score_layer_instance->player_name_input).c_str());
	} 
	else {
		GuiLabel(score_info_bounds, "Du hast es nicht unter die Top 10 geschafft");
	}
	

	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, oldColor);
	GuiSetStyle(DEFAULT, TEXT_SIZE, oldSize);
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, oldAlign);

	int margin	   = 20;
	int btn_height = 24;
	int btn_width  = 80;

	float y = (vp->viewport_base_size().y - btn_height) - margin;

	if ( GuiButton(Rectangle{np.x + margin * sk, np.y + y * sk, btn_width * sk, btn_height * sk}, "Mainmenu") ) {
		reset_state();
		gApp()->call_later([]() { gApp()->pop_layer<DeathScreenLayer>(); });
		gApp()->call_later([]() { gApp()->pop_layer<ScoreLayer>(); });
		gApp()->call_later([]() { gApp()->switch_layer<AppLayer, MainMenuLayer>(); });
	}
	if ( GuiButton(Rectangle{(np.x + btn_width + (margin / 2)) * sk, np.y + y * sk, btn_width * sk, btn_height * sk},
				 "Scoreboard"
		 ) ) {
		reset_state();

		gApp()->call_later([]() { gApp()->pop_layer<DeathScreenLayer>(); });
		gApp()->call_later([]() { gApp()->switch_layer<AppLayer, ScoreLayer>(); });
		gApp()->call_later([]() {
			auto score_layer = gApp()->get_layer<ScoreLayer>();
			if ( score_layer )
				score_layer->load_highscore(score_layer->score_filename());
			score_layer->set_retry_button_visible(true);

		});
	}
	x = (vp->viewport_base_size().x - btn_width) - margin;

	if ( !m_score_should_be_saved || m_name_entered ) {
		if ( GuiButton(Rectangle{np.x + x * sk, np.y + y * sk, btn_width * sk, btn_height * sk}, "Retry") ) {
			reset_state();
			
			if ( m_score_layer_instance && m_name_entered ) {
				m_score_layer_instance->save_highscore(m_score_layer_instance->score_filename());
			}
			
			gApp()->call_later([]() { gApp()->pop_layer<DeathScreenLayer>(); });
			gApp()->call_later([]() { gApp()->pop_layer<ScoreLayer>(); });
			gApp()->call_later([]() { gApp()->pop_layer<AppLayer>(); });
			gApp()->call_later([]() { 
				gApp()->game_director_ref().reset(new FortunaDirector()); 
			});
			gApp()->call_later([]() { gApp()->push_layer<AppLayer>(); });
		}
	}
}

void DeathScreenLayer::set_score_should_be_saved(bool should_be_saved)
{
	m_score_should_be_saved = should_be_saved;
}

void DeathScreenLayer::set_score_layer_instance(ScoreLayer* score_layer)
{
	m_score_layer_instance = score_layer;
}

bool DeathScreenLayer::score_should_be_saved() const
{
	return m_score_should_be_saved;
}

void DeathScreenLayer::reset_state()
{
	m_score_should_be_saved = false;
	m_name_entered = false;
	m_score_layer_instance = nullptr;
}