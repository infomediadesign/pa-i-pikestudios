//
// Created by rikab on 29/01/2026.
//
#include <entities/director.h>
#include <layers/deathscreenlayer.h>
#include <layers/mainmenulayer.h>
#include <layers/scorelayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raygui.h>

void DeathScreenLayer::on_update(float dt)
{
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

	int oldColor = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
	int oldSize	 = GuiGetStyle(DEFAULT, TEXT_SIZE);
	int oldAlign = GuiGetStyle(LABEL, TEXT_ALIGNMENT);

	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xff0000ff);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 28 * sk);

	GuiLabel(rect, "Du bist gestorben :(");

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * sk);

	GuiLabel(score, ("Score: " + bounty_text).c_str());

	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, oldColor);
	GuiSetStyle(DEFAULT, TEXT_SIZE, oldSize);
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, oldAlign);

	int margin	   = 20;
	int btn_height = 24;
	int btn_width  = 80;

	float y = (vp->viewport_base_size().y - btn_height) - margin;

	if ( GuiButton(Rectangle{np.x + margin * sk, np.y + y * sk, btn_width * sk, btn_height * sk}, "Mainmenu") ) {
		gApp()->call_later([]() { gApp()->pop_layer<DeathScreenLayer>(); });
		gApp()->call_later([]() { gApp()->switch_layer<AppLayer, MainMenuLayer>(); });
	}
	if ( GuiButton(
				 Rectangle{static_cast<float>(GetScreenWidth()) / 2 - (btn_width / 2) * sk, np.y + y * sk, btn_width * sk, btn_height * sk},
				 "Scoreboard"
		 ) ) {
		gApp()->push_layer<ScoreLayer>();
		gApp()->call_later([]() {
			auto score_layer = gApp()->get_layer<ScoreLayer>();
			if ( score_layer ) {
				score_layer->layout = ScoreLayer::MAIN_MENU;
			}
		});
		gApp()->pop_layer<AppLayer>();
		gApp()->pop_layer<DeathScreenLayer>();
	}

	x = (vp->viewport_base_size().x - btn_width) - margin;

	if ( GuiButton(Rectangle{np.x + x * sk, np.y + y * sk, btn_width * sk, btn_height * sk}, "Retry") ) {
		gApp()->call_later([]() { gApp()->pop_layer<AppLayer>(); });
		gApp()->call_later([]() { gApp()->switch_layer<DeathScreenLayer, AppLayer>(); });
		gApp()->call_later([]() { gApp()->game_director_ref().reset(new FortunaDirector()); });
	}
}
