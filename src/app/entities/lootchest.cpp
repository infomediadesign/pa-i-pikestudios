#include "lootchest.h"
#include <coordinatesystem.h>
#include <layers/applayer.h>
#include <layers/upgradelayer.h>
#include "entities/player.h"
#include "pscore/viewport.h"

LootChest::LootChest() : PSInterfaces::IEntity("loot_chest")
{
	IRenderable::propose_z_index(-3);
	Vector2 frame_grid{1, 1};
	m_sprite  = PRELOAD_TEXTURE(ident_, "resources/entity/loot_chest.png", frame_grid);
	m_texture = m_sprite->m_s_texture;

	int z_index		  = -3;
	m_anim_controller = PSCore::sprites::SpriteSheetAnimation(m_texture, {{22, 0.05, PSCore::sprites::Backward, z_index}});
	m_anim_controller.add_animation_at_index(0, z_index);
	m_anim_controller.set_animation_at_index(0, 21, z_index);

	m_spawn_anim_playing = true;
}

void LootChest::init(const Vector2& position, std::shared_ptr<LootChest> self)
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

void LootChest::update(float dt)
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
	} else {
		play_idle_anim(dt);
	}
}

void LootChest::render()
{
	if ( is_active_ ) {
		if ( auto& vp = gApp()->viewport() ) {
			vp->draw_in_viewport(m_texture, m_anim_controller.get_source_rectangle(-3).value_or(Rectangle{0}), m_position, m_rotation, WHITE);
		}
	}
}

void LootChest::on_hit()
{
	if ( gApp()->get_layer<UpgradeLayer>() ) {
		gApp()->call_later([]() {
			auto upgrade_layer = gApp()->get_layer<UpgradeLayer>();
			if ( upgrade_layer ) {
				upgrade_layer->m_current_loot_table_values = upgrade_layer->m_loot_table.loot_table_values(3);
				upgrade_layer->print_loot_table_values(upgrade_layer->m_current_loot_table_values);
			}
			auto app_layer = gApp()->get_layer<AppLayer>();
			if ( app_layer )
				app_layer->suspend();
		});
	} else {
		gApp()->call_later([]() { gApp()->push_layer<UpgradeLayer>(); });
		gApp()->call_later([]() {
			auto upgrade_layer = gApp()->get_layer<UpgradeLayer>();
			if ( upgrade_layer ) {
				upgrade_layer->m_current_loot_table_values = upgrade_layer->m_loot_table.loot_table_values(3);
				upgrade_layer->print_loot_table_values(upgrade_layer->m_current_loot_table_values);
			}
			auto app_layer = gApp()->get_layer<AppLayer>();
			if ( app_layer )
				app_layer->suspend();
		});
	}
	set_is_active(false);
}

std::optional<std::vector<Vector2>> LootChest::bounds() const
{
	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_position);
			float scale	   = vp->viewport_scale();
			vp_pos.y += 8 * scale;
			float half						   = 8.0f * scale;
			std::vector<Vector2> hitbox_points = {{-half, -half}, {half, -half}, {half, half}, {-half, half}};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, m_rotation, hitbox_points);
		}

	return std::nullopt;
}

void LootChest::set_position(const Vector2& position)
{
	m_position = position;
}

std::optional<Vector2> LootChest::position() const
{
	return m_position;
}

void LootChest::set_rotation(const float& rotation)
{
	m_rotation = rotation;
}

void LootChest::draw_debug()
{
	if ( auto& vp = gApp()->viewport() ) {
		if ( bounds().has_value() ) {
			for ( int i = 0; i < bounds().value().size(); i++ ) {
				if ( i < bounds().value().size() - 1 ) {
					DrawLineV(bounds().value().at(i), bounds().value().at(i + 1), GREEN);
				} else {
					DrawLineV(bounds().value().at(i), bounds().value().at(0), GREEN);
				}
			}
		}
	}
}

void LootChest::play_spawn_anim(float dt)
{
	if ( m_spawn_anim_playing ) {
		m_anim_controller.update_animation(dt);
		if ( m_anim_controller.get_sprite_sheet_frame_index(-3).value_or(-1) <= 8 ) {
			m_spawn_anim_playing = false;
			m_anim_controller.set_new_animation_framerate(0, 0.15f);
			m_anim_controller.set_animation_at_index(0, 6, -3);
		}
	}
}

void LootChest::play_idle_anim(float dt)
{
	m_anim_controller.update_animation(dt);
	int frame = m_anim_controller.get_sprite_sheet_frame_index(-3).value_or(0);
	if ( frame > 7 ) {
		m_anim_controller.set_animation_at_index(0, 6, -3);
	}
}

void LootChest::set_spawn_anim_playing(bool playing)
{
	m_spawn_anim_playing = playing;
	if ( playing ) {
		m_anim_controller.set_new_animation_framerate(0, 0.05f);
		m_anim_controller.set_animation_at_index(0, 21, -3);
	}
}