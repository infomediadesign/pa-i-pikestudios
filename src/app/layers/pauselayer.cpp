#include "pauselayer.h"
#include <entities/director.h>
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <layers/scorelayer.h>
#include <layers/uilayer.h>
#include <pscore/application.h>
#include <pscore/utils.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <raylib.h>
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
	float screen_width	= static_cast<float>(GetScreenWidth());
	float screen_height = static_cast<float>(GetScreenHeight());

	Vector2 button_size{100 * scale, 50 * scale};

	Rectangle button_rect{origin.x, origin.y, button_size.x, button_size.y};


	GuiSetStyle(DEFAULT, TEXT_SIZE, 6 * scale);
		if ( GuiButton(
					 Rectangle{screen_width / 2 - (40 * scale), screen_height / 2 - (12 * scale) - 20 * scale, 80 * scale, 24 * scale}, "Resume"
			 ) ) {
			gApp()->call_later([]() {
				gApp()->pop_layer<PauseLayer>();
				if ( auto app_layer = gApp()->get_layer<AppLayer>() )
					app_layer->resume();
			});
		}

		if ( GuiButton(Rectangle{screen_width / 2 - (40 * scale), screen_height / 2 - (12 * scale) + 20 * scale, 80 * scale, 24 * scale}, "Main Menu") ) {
			gApp()->call_later([]() {
				gApp()->pop_layer<PauseLayer>();
				gApp()->pop_layer<UILayer>();
				gApp()->switch_layer<AppLayer, MainMenuLayer>();
			});
		}
	}

void PauseLayer::on_update(float dt)
{
	float btn_width	 = 100;
	float btn_height = 50;

	float btn_x = (GetScreenWidth() / 2.f) - (btn_width / 2.f);
	float btn_y = (GetScreenHeight() / 2.f) - (btn_height / 2.f);
}
