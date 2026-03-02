#include "pauselayer.h"
#include <entities/director.h>
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <layers/uilayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <raylib.h>
#include <layers/upgradelayer.h>

PauseLayer::PauseLayer()
{
	Vector2 frame_grid{1, 1};
	m_button = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;

	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( director ) {
		m_director = director;
	}
}

void PauseLayer::on_render()
{
	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	float spacing  = 8;

	float button_width = static_cast<float>(m_button.width);
	float button_height = static_cast<float>(m_button.height);
	Vector2 screen_size = vp->viewport_base_size();
	Vector2 button_pos	= {origin.x / scale + screen_size.x / 2.0f, origin.y / scale + screen_size.y / 2.0f - button_height / 2.0f - 10.0f};

	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0, 0, 0, 255}));

	DrawRectangle(origin.x, origin.y, GetScreenWidth() * scale, GetScreenHeight() * scale, Color{0, 0, 0, 150});
	
	if ( GuiButtonTexture(m_button, button_pos, 0, scale, WHITE, GRAY, "Resume") ) {
		gApp()->call_later([]() {
			gApp()->pop_layer<PauseLayer>();
			if ( auto app_layer = gApp()->get_layer<AppLayer>() )
				app_layer->resume();
			auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
			if ( director ) {
				director->set_is_active(true);
			}
		});
		gApp()->call_later([]() {
			auto upgrade_layer = gApp()->get_layer<UpgradeLayer>();
			if ( upgrade_layer ) {
				if ( auto app_layer = gApp()->get_layer<AppLayer>() )
					app_layer->suspend();
				upgrade_layer->m_layer_is_visible = true;
			}
		});
		HideCursor();
	}

	button_pos.y += button_height + 8.0f;
	
	if ( GuiButtonTexture(m_button, button_pos, 0, scale, WHITE, GRAY, "Main Menu") ) {
		gApp()->call_later([]() {
			gApp()->pop_layer<PauseLayer>();
			gApp()->pop_layer<UILayer>();
			gApp()->pop_layer<UpgradeLayer>();
			gApp()->switch_layer<AppLayer, MainMenuLayer>();

		});
	}

	draw_statistics();
}

void PauseLayer::on_update(float dt)
{
}

void PauseLayer::draw_statistics()
{
	if ( !m_director )
		return;
	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();

	Rectangle stats_base_bounds{origin.x + 20 * scale, origin.y + 20 * scale, 200 * scale, 100 * scale};

	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({255, 255, 255, 255}));
	draw_time(stats_base_bounds, origin, scale);
	draw_kill_stats(stats_base_bounds, origin, scale);

}

std::string PauseLayer::time_to_string(float time) const
{
	int min = static_cast<int>(time) / 60;
	int sec = static_cast<int>(time) % 60;

	return std::format("{:02d}:{:02d}", min, sec);
}

void PauseLayer::draw_time(Rectangle bounds, Vector2 origin, float scale)
{
	auto& vp				  = gApp()->viewport();
	if ( !vp ) {
		return;
	}

	std::string time_text = time_to_string(m_director->statistics().time_played);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 15 * scale);
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	GuiLabel({vp->viewport_base_size().x, bounds.y, bounds.width, bounds.height}, time_text.c_str());
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);
}

void PauseLayer::draw_kill_stats(Rectangle bounds, Vector2 origin, float scale)
{
	bounds.y += 30 * scale;
	GuiSetStyle(DEFAULT, TEXT_SIZE, 15 * scale);
	GuiLabel(bounds, "Kills");
	bounds.y += 20 * scale;
	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);
	if ( m_director->stats.sharks_killed > 0 ) {
		GuiLabel(bounds, ("Sharks: " + std::to_string(m_director->statistics().sharks_killed)).c_str());
	}
	bounds.y += 20 * scale;
	if ( m_director->stats.tentacles_killed > 0 ) {
		GuiLabel(bounds, ("Tentacles: " + std::to_string(m_director->statistics().tentacles_killed)).c_str());
	}
	bounds.y += 20 * scale;
	if ( m_director->stats.hunters_killed > 0 ) {
		GuiLabel(bounds, ("Hunters: " + std::to_string(m_director->statistics().hunters_killed)).c_str());
	}
	bounds.y += 20 * scale;
	if ( m_director->stats.chonky_sharks_killed > 0 ) {
		GuiLabel(bounds, ("Chonky sharks: " + std::to_string(m_director->statistics().chonky_sharks_killed)).c_str());
	}
}

