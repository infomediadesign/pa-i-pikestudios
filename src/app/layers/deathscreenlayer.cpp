#include <entities/director.h>
#include <layers/deathscreenlayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include "applayer.h"

#include "scorelayer.h"

DeathScreenLayer::DeathScreenLayer()
{
	Vector2 frame_grid{1, 1};
	m_button = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;
}

void DeathScreenLayer::on_update(float dt)
{
	if ( m_score_layer_instance ) {
		if ( IsKeyPressed(KEY_ENTER) && m_score_layer_instance->player_name_input.size() > 0 ) {
			gApp()->set_current_player_name(m_score_layer_instance->player_name_input);
			m_name_entered = true;
		}
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


	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xff0000ff);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 28 * sk);

	GuiLabel({np.x + x * sk, np.y + 48 * sk, 400 * sk, 40 * sk}, "You were shattered :(");
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xffffffff);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * sk);

	GuiLabel(score, ("Bounty: " + bounty_text).c_str());
	if ( m_score_should_be_saved ) {
		GuiLabel(score_info_bounds, "You made it into the top 10!");

		float input_row_y  = 200;
		float input_height = 30;
		Rectangle name_prompt_rect{np.x + x * sk, np.y + input_row_y * sk, 200 * sk, input_height * sk};
		Rectangle input_field_rect{np.x + (x + 170) * sk, np.y + input_row_y * sk, 120 * sk, input_height * sk};

		GuiLabel(name_prompt_rect, "Enter your name:");

		if ( !m_name_entered ) {
			GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt({0, 0, 0, 50}));
			GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt({0, 0, 0, 0}));
			GuiPanel(input_field_rect, NULL);
			GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(WHITE));
			GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(WHITE));
		}
		if ( m_score_layer_instance ) {
			GuiLabel(input_field_rect, (m_score_layer_instance->player_name_input).c_str());
		}

	} else {
		GuiLabel(score_info_bounds, "You didn't make it into the top 10");
	}

	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0x00000ff);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * sk);
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, oldAlign);

	int margin		 = 20;
	float btn_width	 = static_cast<float>(m_button.width);
	float btn_height = static_cast<float>(m_button.height);
	float y			 = vp->viewport_base_size().y - btn_height / 2.0f - margin;

	Vector2 mainmenu_pos = {np.x / sk + margin + btn_width / 2.0f, np.y / sk + y};

	if ( GuiButtonTexture(m_button, mainmenu_pos, 0, sk, WHITE, GRAY, "Mainmenu") ) {
		reset_state();
		gApp()->call_later([]() { gApp()->pop_layer<DeathScreenLayer>(); });
		gApp()->call_later([]() { gApp()->pop_layer<ScoreLayer>(); });
		gApp()->call_later([]() { gApp()->switch_layer<AppLayer, MainMenuLayer>(); });
	}

	if ( !m_score_should_be_saved || m_name_entered ) {
		Vector2 scoreboard_pos = {np.x / sk + margin * 2 + btn_width + btn_width / 2.0f, np.y / sk + y};

		if ( GuiButtonTexture(m_button, scoreboard_pos, 0, sk, WHITE, GRAY, "Scoreboard") ) {
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

		Vector2 retry_pos = {np.x / sk + vp->viewport_base_size().x - margin - btn_width / 2.0f, np.y / sk + y};

		if ( GuiButtonTexture(m_button, retry_pos, 0, sk, WHITE, GRAY, "Retry") ) {
			reset_state();

			if ( m_score_layer_instance && m_name_entered ) {
				m_score_layer_instance->save_highscore(m_score_layer_instance->score_filename());
			}

			gApp()->call_later([]() { gApp()->pop_layer<DeathScreenLayer>(); });
			gApp()->call_later([]() { gApp()->pop_layer<ScoreLayer>(); });
			gApp()->call_later([]() { gApp()->pop_layer<AppLayer>(); });
			gApp()->call_later([]() { gApp()->game_director_ref().reset(new FortunaDirector()); });
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
	m_name_entered			= false;
	m_score_layer_instance	= nullptr;
	gApp()->get_layer<AppLayer>()->set_can_open_pause_menu(true);
}
