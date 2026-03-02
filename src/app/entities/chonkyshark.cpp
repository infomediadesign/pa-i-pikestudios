#include <coordinatesystem.h>
#include <entities/chonkyshark.h>
#include <entities/shark.h>
#include <pscore/settings.h>
#include <pscore/viewport.h>
#include "entities/director.h"

enum Type { UwUNormal = 2, UwUHurt, Normal, Hurt, Fin };

void ChonkyShark::on_hit()
{
	if ( m_hurt && m_remaining_iframe_time <= 0.0f ) {
		set_is_active(false);
		auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
		if ( !director ) {
			return;
		}
		if ( m_marked ) {
			director->spawn_loot_chest(m_pos);
		}
		return;
	}

	m_hurt					= true;
	m_remaining_iframe_time = m_iframe_duration;
	int current_frame		= m_animation_controller.get_sprite_sheet_frame_index(-1).value_or(0);

	if ( m_uwu ) {
		m_animation_controller.set_animation_at_index(UwUHurt, current_frame, -1);
	}
	else {
		m_animation_controller.set_animation_at_index(Hurt, current_frame, -1);
	}
}

std::optional<std::vector<Vector2>> ChonkyShark::bounds() const
{
	if ( is_active_ ) {
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_pos);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points = {
					{19 * scale, 0 * scale}, {0 * scale, 12 * scale}, {-19 * scale, 0 * scale}, {0 * scale, -12 * scale}
			};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, m_shark_rotation, hitbox_points);
		}
	}

	return std::nullopt;
}

ChonkyShark::ChonkyShark() : Shark(), PSInterfaces::IEntity("chonky_shark")
{
	m_uwu = determine_uwu_();

	int current_frame = m_animation_controller.get_sprite_sheet_frame_index(-1).value_or(0);

	if ( m_uwu ) {
		m_animation_controller.set_animation_at_index(UwUNormal, current_frame, -1);
	}
	else {
		m_animation_controller.set_animation_at_index(Normal, current_frame, -1);
	}

	m_animation_controller.set_animation_at_index(Fin, current_frame, 1);

	m_smear_origin = {7, 0.5};
};

ChonkyShark::~ChonkyShark() {};

void ChonkyShark::update(float dt)
{
	Shark::update(dt);

	if ( m_remaining_iframe_time > 0.0f )
		m_remaining_iframe_time -= dt;
}

void ChonkyShark::set_is_active(bool active)
{
	Shark::set_is_active(active);

	if ( active ) {
		m_hurt = false;
		m_uwu  = determine_uwu_();

		if ( m_uwu ) {
			m_animation_controller.set_animation_at_index(UwUNormal, 0, -1);
		}
		else {
			m_animation_controller.set_animation_at_index(Normal, 0, -1);
		}

		m_animation_controller.set_animation_at_index(Fin, 0, 1);
	}
}

bool ChonkyShark::determine_uwu_()
{
	return PSUtils::gen_rand(0, 100) <= CFG_VALUE("chonky_uwu_chance", 10);
}
