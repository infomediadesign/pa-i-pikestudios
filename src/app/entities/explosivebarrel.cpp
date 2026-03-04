#include <coordinatesystem.h>
#include <entities/explosivebarrel.h>
#include <iostream>
#include <layers/applayer.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <raylib.h>
#include "pscore/sprite.h"

ExplosiveBarrel::ExplosiveBarrel() : PSInterfaces::IEntity("explosive_barrel")
{
	SetShaderValue(m_flash_shader, GetShaderLocation(m_flash_shader, "flash_color"), &m_flash_color, SHADER_UNIFORM_VEC4);
	m_flash_alpha_location = GetShaderLocation(m_flash_shader, "flash_alpha");

	Vector2 frame_grid{1, 8};
	PRELOAD_TEXTURE(ident_, "resources/vfx/explosion_barrel.png", frame_grid);
	float frame_time	   = m_damage_time / 8;
	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(FETCH_SPRITE_TEXTURE(ident_), {{8, frame_time, PSCore::sprites::Forward, 1}});
	m_animation_controller.add_animation_at_index(0, 1);
	propose_z_index(-1);

	m_global_sfx_volume = gApp()->sound_volume(PSCore::Application::SoundType::SFX).value_or(50);
};

ExplosiveBarrel::~ExplosiveBarrel()
{
	UnloadShader(m_flash_shader);

	UnloadSound(m_explode_sound);
}

void ExplosiveBarrel::update(float dt)
{
	m_timer += dt;
	if ( is_exploding() ) {
		if ( m_timer > m_detonation_time + m_damage_time )
			set_is_active(false);

		m_animation_controller.update_animation(dt);
		if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
			m_collider->check_collision(app_layer->entities());
		}

		if ( m_can_play_sound ) {
			m_can_play_sound = false;

			if ( IsSoundPlaying(m_explode_sound) )
				StopSound(m_explode_sound);

			int random_volume = PSUtils::gen_rand(static_cast<int>(m_volume_boundary.x), static_cast<int>(m_volume_boundary.y));
			int random_pitch  = PSUtils::gen_rand(static_cast<int>(m_pitch_boundary.x), static_cast<int>(m_pitch_boundary.y));

			SetSoundVolume(m_explode_sound, std::min((m_global_sfx_volume / 100) * (m_shoot_volume + static_cast<float>(random_volume) / 100), 1.0f));
			SetSoundPitch(m_explode_sound, m_shoot_pitch + static_cast<float>(random_pitch) / 100);

			PlaySound(m_explode_sound);
		}

		return;
	}

	float n_flash_lerp_scale = m_flash_lerp_scale / m_detonation_time;
	m_flash_alpha			 = 0.5f * cosf(m_timer * sqrtf(m_timer) * n_flash_lerp_scale * sqrtf(n_flash_lerp_scale)) + 0.5;

	SetShaderValue(m_flash_shader, m_flash_alpha_location, &m_flash_alpha, SHADER_UNIFORM_FLOAT);
};

void ExplosiveBarrel::render()
{
	if ( auto& vp = gApp()->viewport() ) {
		Vector2 pos_global = vp->position_viewport_to_global(m_position);
		if ( is_exploding() ) {
			propose_z_index(100);

			vp->draw_in_viewport(
					FETCH_SPRITE_TEXTURE(ident_), m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_position, 0, WHITE
			);
		}
		else {
			propose_z_index(-1);

			DrawCircleV(pos_global, Lerp(0,m_explosion_radius, m_timer / m_detonation_time), {200,0,0,75});

			BeginShaderMode(m_flash_shader);

			DrawCircleV(pos_global, 3 * vp->viewport_scale(), WHITE);

			EndShaderMode();
		}
	}
};

void ExplosiveBarrel::set_is_active(bool active)
{
	is_active_ = active;

	if ( is_active_ ) {
		m_timer		  = 0;
		m_flash_alpha = 0;
		m_animation_controller.set_animation_at_index(0, 0, 1);
		m_can_play_sound = true;
	}
};

void ExplosiveBarrel::init(std::shared_ptr<ExplosiveBarrel> self, const Vector2& position)
{
	m_position = position;

	m_collider = std::make_unique<PSCore::collision::EntityCollider>(self);
	m_collider->register_collision_handler([this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
		if ( auto locked = other.lock() ) {
			locked->on_hit();
		}
	});
}

std::optional<std::vector<Vector2>> ExplosiveBarrel::bounds() const
{
	if ( is_active_ && is_exploding() )
		return generate_circle_polygon_(m_position, m_explosion_radius);

	return std::nullopt;
}

std::vector<Vector2> ExplosiveBarrel::generate_circle_polygon_(Vector2 center, float radius, int segments) const
{
	std::vector<Vector2> points;
	points.reserve(segments);

	const float angleStep = 2.0f * PI / segments;

	Vector2 glob_center;
	if ( auto& vp = gApp()->viewport() ) {
		float scale = vp->viewport_scale();
		glob_center = vp->position_viewport_to_global(center);

		float glob_radius = radius * scale;
		for ( int i = 0; i < segments; ++i ) {
			float angle = i * angleStep;
			points.push_back({0 + glob_radius * std::cos(angle), 0 + glob_radius * std::sin(angle)});
		}
	}

	return coordinatesystem::points_relative_to_globle_rightup(glob_center, 0, points);
}

void ExplosiveBarrel::draw_debug()
{
	if ( bounds().has_value() ) {
		for ( int i = 0; i < bounds().value().size(); i++ ) {
			if ( i < bounds().value().size() - 1 ) {
				DrawLineV(bounds().value().at(i), bounds().value().at(i + 1), GREEN);
			} else {
				DrawLineV(bounds().value().at(i), bounds().value().at(0), GREEN);
			}
		}
	}
};

bool ExplosiveBarrel::is_exploding() const
{
	return m_timer >= m_detonation_time;
}
