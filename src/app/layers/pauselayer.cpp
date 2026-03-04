#include "pauselayer.h"
#include <entities/director.h>
#include <layers/applayer.h>
#include <layers/mainmenulayer.h>
#include <layers/uilayer.h>
#include <layers/upgradelayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <raylib.h>
#include "layers/optionslayer.h"
#include "pscore/utils.h"

PauseLayer::PauseLayer()
{
	Vector2 frame_grid{1, 1};
	m_button = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;

	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( director ) {
		m_director = director;
		init_stat_strings();
	}
}

void PauseLayer::on_render()
{
	if (!active)
		return;
	
	auto& vp	   = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	float scale	   = vp->viewport_scale();
	float spacing  = 8;

	float button_width	= static_cast<float>(m_button.width);
	float button_height = static_cast<float>(m_button.height);
	Vector2 screen_size = vp->viewport_base_size();
	Vector2 button_pos	= {origin.x / scale + screen_size.x / 2.0f, origin.y / scale + screen_size.y / 2.0f - button_height / 2.0f - 30.0f};

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
		gApp()->play_ui_sound(0);
	}

	button_pos.y += button_height + 8.0f;

	if ( GuiButtonTexture(m_button, button_pos, 0, scale, WHITE, GRAY, "Main Menu") ) {
		gApp()->call_later([]() {
			gApp()->pop_layer<PauseLayer>();
			gApp()->pop_layer<UILayer>();
			gApp()->pop_layer<UpgradeLayer>();
			gApp()->switch_layer<AppLayer, MainMenuLayer>();
		});
		gApp()->play_ui_sound(0);
	}

	button_pos.y += button_height + 8.0f;

	if ( GuiButtonTexture(m_button, button_pos, 0, scale, WHITE, GRAY, "Options") ) {
		gApp()->call_later([this]() {
			OptionsLayer* layer = gApp()->push_layer<OptionsLayer>();
			layer->set_exit_btn_function("Save", [this] {
				gApp()->call_later([this] {
					gApp()->pop_layer<OptionsLayer>();
					this->resume();
				});
			});
		});
		gApp()->play_ui_sound(0);
		suspend();
	}

	button_pos.y += button_height + 8.0f;

	if ( GuiButtonTexture(m_button, button_pos, 0, scale, WHITE, GRAY, "Retry") ) {
		gApp()->call_later([]() {
			gApp()->pop_layer<AppLayer>();
			gApp()->pop_layer<UILayer>();
			gApp()->switch_layer<PauseLayer, AppLayer>();
			gApp()->game_director_ref().reset(new FortunaDirector());
		});
		HideCursor();
		gApp()->play_ui_sound(0);
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

	m_stats_base_bounds = {origin.x + 20 * scale, origin.y + 20 * scale, 200 * scale, 100 * scale};

	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({255, 255, 255, 255}));
	draw_time(scale);
	draw_kill_stats(scale);
	draw_player_stats(scale);


	m_stats_base_bounds = {origin.x + 20 * scale, origin.y + 20 * scale, 200 * scale, 100 * scale};
}

void PauseLayer::draw_time(float scale)
{
	auto& vp = gApp()->viewport();
	if ( !vp ) {
		return;
	}
	Vector2 origin = vp->viewport_origin();
	float center_x = origin.x + (vp->viewport_base_size().x / 2.0f) * scale;
	float pos_x	   = center_x - m_stats_base_bounds.width / 2.0f;

	std::string time_text = PSUtils::time_to_string(m_director->statistics().time_played);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20 * scale);
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	GuiLabel({pos_x, m_stats_base_bounds.y, m_stats_base_bounds.width, m_stats_base_bounds.height}, time_text.c_str());
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);
}

void PauseLayer::draw_kill_stats(float scale)
{
	float vertical_spacing	 = 10 * scale;
	float horizontal_spacing = 75 * scale;

	GuiSetStyle(DEFAULT, TEXT_SIZE, 15 * scale);
	GuiLabel(m_stats_base_bounds, "Kills");
	m_stats_base_bounds.y += 20 * scale;

	for ( int i = 0; i < m_kill_stat_lines.size(); i += 2 ) {
		GuiSetStyle(DEFAULT, TEXT_SIZE, 6 * scale);
		GuiLabel(m_stats_base_bounds, m_kill_stat_lines[i].c_str());
		GuiLabel(
				{m_stats_base_bounds.x + horizontal_spacing, m_stats_base_bounds.y, m_stats_base_bounds.width, m_stats_base_bounds.height},
				m_kill_stat_lines[i + 1].c_str()
		);
		m_stats_base_bounds.y += vertical_spacing;
	}
}

void PauseLayer::draw_player_stats(float scale)
{
	float vertical_spacing	 = 10 * scale;
	float horizontal_spacing = 75 * scale;
	m_stats_base_bounds.y += 30 * scale;
	GuiSetStyle(DEFAULT, TEXT_SIZE, 15 * scale);
	GuiLabel(m_stats_base_bounds, "Player Stats");
	m_stats_base_bounds.y += 20 * scale;

	for ( int i = 0; i < m_player_stat_lines.size(); i += 2 ) {
		GuiSetStyle(DEFAULT, TEXT_SIZE, 6 * scale);
		GuiLabel(m_stats_base_bounds, m_player_stat_lines[i].c_str());
		GuiLabel(
				{m_stats_base_bounds.x + horizontal_spacing, m_stats_base_bounds.y, m_stats_base_bounds.width, m_stats_base_bounds.height},
				m_player_stat_lines[i + 1].c_str()
		);
		m_stats_base_bounds.y += vertical_spacing;
	}
}

void PauseLayer::init_stat_strings()
{
	m_player_stat_lines.clear();
	m_kill_stat_lines.clear();

	// init player stats
	m_player_stat_lines.push_back("Speed:");
	m_player_stat_lines.push_back(std::format("{:.2f}", m_director->player_max_velocity()));
	m_player_stat_lines.push_back("Turn Speed:");
	m_player_stat_lines.push_back(std::format("{:.2f}", m_director->player_input_rotation_mult()));
	m_player_stat_lines.push_back("Fire Rate:");
	m_player_stat_lines.push_back(std::format("{:.2f}s", m_director->player_current_fire_rate()));
	m_player_stat_lines.push_back("Projectile Speed:");
	m_player_stat_lines.push_back(std::format("{:.2f}", m_director->player_current_projectile_speed()));
	m_player_stat_lines.push_back("Fire Range:");
	m_player_stat_lines.push_back(std::format("{:.2f}", m_director->player_current_fire_range()));
	m_player_stat_lines.push_back("Piercing Chance:");
	m_player_stat_lines.push_back(std::format("{:.2f}%", m_director->player_piercing_chance()));
	m_player_stat_lines.push_back("Luck:");
	m_player_stat_lines.push_back(std::format("{:.2f}%", m_director->player_luck() * 100));
	if ( m_director->player_projectile_amount() > 1 ) {
		m_player_stat_lines.push_back("Multi Shot:");
		m_player_stat_lines.push_back(std::format("{}", m_director->player_projectile_amount()));
	}
	if ( m_director->players()[0]->cannon_container().size() > 4 ) {
		m_player_stat_lines.push_back("Cannons:");
		m_player_stat_lines.push_back(std::format("{}", m_director->players()[0]->cannon_container().size()));
	}
	if ( m_director->players()[0]->explosive_barrels_enabled() ) {
		m_player_stat_lines.push_back("Explosive Barrels:");
		m_player_stat_lines.push_back("enabled");
	}





	// init kill stats
	if ( m_director->statistics().sharks_killed > 0 ) {
		m_kill_stat_lines.push_back("Sharks:");
		m_kill_stat_lines.push_back(std::format("{}", m_director->statistics().sharks_killed));
	}
	if ( m_director->statistics().tentacles_killed > 0 ) {
		m_kill_stat_lines.push_back("Tentacles:");
		m_kill_stat_lines.push_back(std::format("{}", m_director->statistics().tentacles_killed));
	}
	if ( m_director->statistics().hunters_killed > 0 ) {
		m_kill_stat_lines.push_back("Hunters:");
		m_kill_stat_lines.push_back(std::format("{}", m_director->statistics().hunters_killed));
	}
	if ( m_director->statistics().chonky_sharks_killed > 0 ) {
		m_kill_stat_lines.push_back("Chonky sharks:");
		m_kill_stat_lines.push_back(std::format("{}", m_director->statistics().chonky_sharks_killed));
	}
}
