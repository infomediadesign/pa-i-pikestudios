#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <layers/scorelayer.h>
#include <entities/director.h>
#include <raygui.h>
#include <raylib.h>

static const int STANDART_TEXT_SIZE = 10;

void MainMenuLayer::on_update(float dt)
{
}

void MainMenuLayer::on_render()
{
	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	float spacing = 10 * scale;

	Vector2 button_size{100 * scale, 50 * scale};
	
	Rectangle button_rect{origin.x, origin.y, button_size.x, button_size.y};
	auto next_btn_rect = [&button_rect, spacing]() {
		Rectangle rec{button_rect.x, button_rect.y + button_rect.height + spacing, button_rect.width, button_rect.height};
		button_rect = rec;
		return rec;
	};

	GuiSetStyle(DEFAULT, TEXT_SIZE, 6 * scale);

	if ( GuiButton(button_rect, "Start Game") ) {
		gApp()->call_later([]() {
			if ( auto& director = gApp()->game_director_ref() ) {
				director.reset(new FortunaDirector());
			}
		});
		gApp()->call_later([]() { gApp()->switch_layer<MainMenuLayer, AppLayer>(); });
	}
	if ( GuiButton(next_btn_rect(), "Options") ) {
		gApp()->call_later([]() { PS_LOG(LOG_INFO, "Clicked Settings"); });
	}
	if ( GuiButton(next_btn_rect(), "Leader Board") ) {
		gApp()->call_later([]() { PS_LOG(LOG_INFO, "Clicked Leader Board"); });
		gApp()->call_later([]() { gApp()->switch_layer<MainMenuLayer, ScoreLayer>(); });
		gApp()->call_later([]() { 
			auto score_layer = gApp()->get_layer<ScoreLayer>();
			if ( score_layer )
				score_layer->load_highscore("noahistgay.txt");
		});
	}
	if ( GuiButton(next_btn_rect(), "Quit") ) {
		gApp()->stop();
	}

	GuiSetStyle(DEFAULT, TEXT_SIZE, STANDART_TEXT_SIZE);
}
