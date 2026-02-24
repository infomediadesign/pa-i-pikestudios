#include <coordinatesystem.h>
#include <entities/chonkyshark.h>
#include <entities/shark.h>
#include <pscore/viewport.h>

void ChonkyShark::on_hit()
{
	if ( hurt ) {
		Shark::on_hit();
		return;
	}

	hurt = true;
}

std::optional<std::vector<Vector2>> ChonkyShark::bounds() const
{
	if ( is_active_ ) {
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_pos);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points = {{15 * scale, 0 * scale}, {0 * scale, 8 * scale}, {-15 * scale, 0 * scale}, {0 * scale, -8 * scale}};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, m_shark_rotation, hitbox_points);
		}
	}

	return std::nullopt;
}

ChonkyShark::ChonkyShark() : Shark(), PSInterfaces::IEntity("chonky_shark")
{
	Vector2 frame_grid{9, 2};
	PRELOAD_TEXTURE(ident_, "resources/entity/chonky_shark.png", frame_grid);
	m_shark_sprite = FETCH_SPRITE(ident_);

	enum Type { UwUNormal = 0, UwUHurt, Normal, Hurt, Fin };

	std::vector<PSCore::sprites::SpriteSheetData> sp_data{
			{9, 0.1, PSCore::sprites::Forward, -1}, {9, 0.1, PSCore::sprites::Forward, -1}, {9, 0.1, PSCore::sprites::Forward, -1},
			{9, 0.1, PSCore::sprites::Forward, -1}, {9, 0.1, PSCore::sprites::Forward, 1},
	};

	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(FETCH_SPRITE_TEXTURE(ident_), sp_data);

	m_animation_controller.add_animation_at_index(Normal, -1);
	m_animation_controller.add_animation_at_index(Fin, 1);
};

ChonkyShark::~ChonkyShark() {};
