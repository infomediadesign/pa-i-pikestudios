#include "entities/gemstone.h"
#include "entities/player.h"
#include "pscore/viewport.h"
#include <layers/applayer.h>
#include <entities/director.h>
#include <coordinatesystem.h>
#include "pscore/utils.h"

Gemstone::Gemstone() : PSInterfaces::IEntity("gemstone")
{
	IRenderable::propose_z_index(m_z_index);
	Vector2 frame_grid{1, 1};
	m_sprite  = PRELOAD_TEXTURE(ident_, "resources/entity/gemstone.png", frame_grid);
	m_texture = m_sprite->m_s_texture;

	m_anim_controller = PSCore::sprites::SpriteSheetAnimation(m_texture, {
		{14, 0.1, PSCore::sprites::Forward, m_z_index},
		{14, 0.1, PSCore::sprites::Forward, m_z_index},
		{14, 0.05, PSCore::sprites::Backward, m_z_index},
	});
	m_anim_controller.add_animation_at_index(2, m_z_index);
	m_anim_controller.set_animation_at_index(2, 13, m_z_index);

	m_current_idle_anim  = 0;
	m_spawn_anim_playing = false;

	m_global_sfx_volume = gApp()->sound_volume(PSCore::Application::SoundType::SFX).value_or(50);
}

Gemstone::~Gemstone()
{
	UnloadSound(m_splash_sound);
}

void Gemstone::init(const Vector2& position, std::shared_ptr<Gemstone> self)
{
	set_position(position);
	m_collider = std::make_unique<PSCore::collision::EntityCollider>(self);
	m_collider->register_collision_handler([this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
		if ( auto locked = other.lock() ) {
			if ( auto player = dynamic_cast<Player*>(locked.get()) ) {
				on_hit();
			}
		}
	});
}

void Gemstone::update(float dt)
{
	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		m_collider->check_collision(app_layer->entities(), [this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point) {
			if ( auto locked = other.lock() ) {
				if ( locked->ident() == "player" ) {
					return true;
				}
			}
			return false;
		});
	}
	if ( m_spawn_anim_playing ) {
		play_spawn_anim(dt);
	} 
	else {
		play_idle_anim(dt);
	}
}

void Gemstone::render()
{
	if ( is_active_ ) {
		if ( auto& vp = gApp()->viewport() ) {
			vp->draw_in_viewport(m_texture, m_anim_controller.get_source_rectangle(m_z_index).value_or(Rectangle{0}), m_position, m_rotation, WHITE);
		}
	}
}

void Gemstone::on_hit()
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( director ) {
		director->set_reroll_amount(director->reroll_amount() + 1);
	}
	set_is_active(false);
}

void Gemstone::play_spawn_anim(float dt)
{
	if ( m_spawn_anim_playing ) {
		m_anim_controller.update_animation(dt);
		if ( m_anim_controller.get_sprite_sheet_frame_index(m_z_index).value_or(-1) == 0 ) {
			m_spawn_anim_playing = false;
			m_can_play_spawn_sound = true;
			m_current_idle_anim  = 0;
			m_anim_controller.set_animation_at_index(m_current_idle_anim, 0, m_z_index);
		}
		if ( m_anim_controller.get_sprite_sheet_frame_index(m_z_index).value_or(-1) == 7 && m_can_play_spawn_sound ) {
			m_can_play_spawn_sound = false;

			int random_volume = PSUtils::gen_rand(m_volume_boundary.x, m_volume_boundary.y);
			int random_pitch  = PSUtils::gen_rand(m_pitch_boundary.x, m_pitch_boundary.y);

			SetSoundVolume(m_splash_sound, std::min((m_global_sfx_volume / 100) * (m_splash_volume + static_cast<float>(random_volume) / 100), 1.0f));
			SetSoundPitch(m_splash_sound, m_splash_pitch + static_cast<float>(random_pitch) / 100);

			PlaySound(m_splash_sound);
		}
	}
}

void Gemstone::play_idle_anim(float dt)
{
	m_anim_controller.update_animation(dt);

	if ( m_anim_controller.get_sprite_sheet_frame_index(m_z_index).value_or(-1) == 13 ) {
		if ( m_can_change_anim ) {
			m_can_change_anim = false;

			int rand = PSUtils::gen_rand(0, 3);
			if ( rand > 0 ) {
				m_current_idle_anim = 0;
			} else {
				m_current_idle_anim = 1;
			}
			m_anim_controller.set_animation_at_index(m_current_idle_anim, 0, m_z_index);
		}
	}
	else {
		m_can_change_anim = true;
	}
}

void Gemstone::set_spawn_anim_playing(bool playing)
{
	m_spawn_anim_playing = playing;
	m_anim_controller.set_animation_at_index(2, 13, m_z_index);
}

std::optional<std::vector<Vector2>> Gemstone::bounds() const
{
	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_position);
			float scale	   = vp->viewport_scale();
			float half						   = 8.0f * scale;
			std::vector<Vector2> hitbox_points = {{-half, -half}, {half, -half}, {half, half}, {-half, half}};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, m_rotation, hitbox_points);
		}

	return std::nullopt;
}

void Gemstone::set_position(const Vector2& position)
{
	m_position = position;
}

std::optional<Vector2> Gemstone::position() const
{
	return m_position;
}

void Gemstone::set_rotation(const float& rotation)
{
	m_rotation = rotation;
}

void Gemstone::draw_debug()
{
}