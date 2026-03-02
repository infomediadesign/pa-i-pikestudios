#include "applayer.h"
#include "debuglayer.h"
#include "pauselayer.h"
#include "psinterfaces/entity.h"

#include <algorithm>
#include <memory>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raylib.h>
#include <layers/upgradelayer.h>

#include <psinterfaces/renderable.h>

#include <entities/player.h>
#include <layers/uilayer.h>
#include <misc/mapborderinteraction.h>
#include <utility>

class Water : public PSInterfaces::IRenderable
{
public:
	Water() : PSInterfaces::IEntity("water")
	{
		SetShaderValue(m_wave_shader, GetShaderLocation(m_wave_shader, "size"), &m_size, SHADER_UNIFORM_VEC2);
		SetShaderValue(m_wave_shader, GetShaderLocation(m_wave_shader, "main_freq"), &m_main_frequency, SHADER_UNIFORM_VEC2);
		SetShaderValue(m_wave_shader, GetShaderLocation(m_wave_shader, "main_amp"), &m_main_amplitude, SHADER_UNIFORM_VEC2);
		SetShaderValue(m_wave_shader, GetShaderLocation(m_wave_shader, "main_vel"), &m_main_velocity, SHADER_UNIFORM_VEC2);
		SetShaderValue(m_wave_shader, GetShaderLocation(m_wave_shader, "sub_freq"), &m_sub_frequency, SHADER_UNIFORM_VEC2);
		SetShaderValue(m_wave_shader, GetShaderLocation(m_wave_shader, "sub_amp"), &m_sub_amplitude, SHADER_UNIFORM_VEC2);
		SetShaderValue(m_wave_shader, GetShaderLocation(m_wave_shader, "sub_vel"), &m_sub_velocity, SHADER_UNIFORM_VEC2);

		m_shader_time_location = GetShaderLocation(m_wave_shader, "time");
		SetShaderValue(m_wave_shader, m_shader_time_location, &m_shader_time, SHADER_UNIFORM_FLOAT);
	}

	~Water() override
	{
		UnloadTexture(m_water);

		UnloadShader(m_wave_shader);
	}

	void update(float dt) override
	{
		m_shader_time += dt;
		SetShaderValue(m_wave_shader, m_shader_time_location, &m_shader_time, SHADER_UNIFORM_FLOAT);
	}

	void render() override
	{
		BeginShaderMode(m_wave_shader);
		if ( auto& vp = gApp()->viewport() ) {
			DrawTextureEx(m_water, vp->viewport_origin(), 0, vp->viewport_scale(), clr);
		}
		EndShaderMode();
	}

private:
	Color clr		  = {255, 255, 255, 150};
	Texture2D m_water = LoadTexture("resources/environment/water.png");

	// Shader
	Shader m_wave_shader	   = LoadShader(NULL, "resources/shader/wave.fs");
	Vector2 m_size			   = {4000, 3000};
	Vector2 m_main_frequency   = {0.01, 0.005};
	Vector2 m_main_amplitude   = {10, 20};
	Vector2 m_main_velocity	   = {2, 1};
	Vector2 m_sub_frequency	   = {0, 0};
	Vector2 m_sub_amplitude	   = {0, 0};
	Vector2 m_sub_velocity	   = {0, 0};
	int m_shader_time_location = 0;
	float m_shader_time		   = 0;
};

class AppLayerPriv
{
	friend class AppLayer;

	std::shared_ptr<Water> water = std::make_shared<Water>();
};

AppLayer::AppLayer()
{
	_p = std::make_unique<AppLayerPriv>();
	// Push the UI layer

	gApp()->call_later([]() {
		auto app = PSCore::Application::get();
		if ( !app->get_layer<UILayer>() ) {
			app->push_layer<UILayer>();
		}

		if ( auto director = dynamic_cast<FortunaDirector*>(app->game_director()) )
			director->initialize_entities();
	});

	_p->water->propose_z_index(-20);
	register_entity(_p->water, true);
}

AppLayer::~AppLayer()
{
}

void AppLayer::on_update(const float dt)
{
	{ // TODO: Refactor this scope out of the App layer
		auto app = PSCore::Application::get();
#ifndef NDEBUG
		if ( IsKeyPressed(KEY_F3) ) {
			if ( app->get_layer<DebugLayer>() )
				app->pop_layer<DebugLayer>();
			else
				app->push_layer<DebugLayer>();
		}
#endif
		if ( m_can_open_pause_menu ) {
			if ( IsKeyPressed(KEY_ESCAPE) ) {
				auto& director = gApp()->game_director_ref();
				if ( app->get_layer<PauseLayer>() ) {
					app->pop_layer<PauseLayer>();
					if ( auto app_layer = app->get_layer<AppLayer>() ) {
						app_layer->resume();
						director->set_is_active(true);
							if ( auto upgrade_layer = gApp()->get_layer<UpgradeLayer>() ) {
								upgrade_layer->m_layer_is_visible = true;
								app_layer->suspend();
								director->set_is_active(false);
							}
					}

				} else {
					app->push_layer<PauseLayer>();
					if ( auto app_layer = app->get_layer<AppLayer>() ) {
						gApp()->call_later([]() {
							if ( auto upgrade_layer = gApp()->get_layer<UpgradeLayer>() ) {
								upgrade_layer->m_layer_is_visible = false;
							}
						});
						app_layer->suspend();
						director->set_is_active(false);
					}
				}
			}
		}
	}

	if ( !active )
		return;

	for ( const auto& entity: const_cast<const AppLayer*>(this)->entities() ) {
		if ( auto locked_entity = entity.lock(); locked_entity->is_active() ) {
			locked_entity->update(dt);

			if ( auto player = dynamic_cast<Player*>(locked_entity.get()) ) {
				misc::map::map_border_wrap_around(*player);
			}
		}
	}
}

void AppLayer::on_render()
{
	if ( renderer_ )
		renderer_->render();
}

void AppLayer::set_can_open_pause_menu(bool can_open)
{
	m_can_open_pause_menu = can_open;
}

bool AppLayer::can_open_pause_menu() const
{
	return m_can_open_pause_menu;
}
