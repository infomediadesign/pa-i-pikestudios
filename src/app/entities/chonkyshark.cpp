#include <coordinatesystem.h>
#include <entities/chonkyshark.h>
#include <entities/shark.h>
#include <pscore/settings.h>
#include <pscore/viewport.h>
#include "entities/director.h"

enum Type { UwUNormal = 0, UwUHurt, Normal, Hurt, Fin };

void ChonkyShark::on_hit()
{
	if ( m_hurt && m_remaining_iframe_time <= 0.0f ) {
		set_is_active(false);
		auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
		if ( !director ) {
			return;
		}
		director->m_b_bounty.add_bounty(director->m_b_bounty_amounts.big_shark_bounty);
		if ( m_marked ) {
			director->spawn_loot_chest(m_pos);
		}
		return;
	}

	m_hurt					= true;
	m_remaining_iframe_time = m_iframe_duration;
	int current_frame		= m_animation_controller.get_sprite_sheet_frame_index(-1).value_or(0);
	m_animation_controller.set_animation_at_index(m_uwu ? UwUHurt : Hurt, current_frame, -1);
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
	Vector2 frame_grid{9, 5};
	auto tex = gApp()->sprite_loader()->preload(ident_, "resources/entity/chonky_shark.png", frame_grid, true);
	m_shark_sprite.swap(tex);

	std::vector<PSCore::sprites::SpriteSheetData> sp_data{
			{9, 0.1, PSCore::sprites::Forward, -1}, {9, 0.1, PSCore::sprites::Forward, -1}, {9, 0.1, PSCore::sprites::Forward, -1},
			{9, 0.1, PSCore::sprites::Forward, -1}, {9, 0.1, PSCore::sprites::Forward, 1},
	};

	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(tex->m_s_texture, sp_data);

	m_uwu = determine_uwu_();
	m_animation_controller.add_animation_at_index(m_uwu ? UwUNormal : Normal, -1);
	m_animation_controller.add_animation_at_index(Fin, 1);

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
		m_animation_controller.set_animation_at_index(m_uwu ? UwUNormal : Normal, 0, -1);
		m_animation_controller.set_animation_at_index(Fin, 0, 1);
	}
}

bool ChonkyShark::determine_uwu_()
{
	return PSUtils::gen_rand(0, 100) <= CFG_VALUE("chonky_uwu_chance", 10);
}
