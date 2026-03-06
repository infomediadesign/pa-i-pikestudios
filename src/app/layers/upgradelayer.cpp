#include "layers/upgradelayer.h"
#include <entities/director.h>
#include <format>
#include <raylib.h>
#include <string>
#include "pscore/sprite.h"
#include "pscore/viewport.h"
#include "raygui.h"

float easeInElastic(float x)
{
	const float c4 = (2 * PI) / CFG_VALUE<float>("upgrade_menu_frequency", 5.5f);

	int stiffness = CFG_VALUE<int>("upgrade_menu_elastic_stiffness", 15);
	return x == 0 ? 0 : x == 1 ? 1 : -pow(2, stiffness * x - stiffness) * sin((x * 10 - 10.75) * c4);
}

float easeOutElastic(float x)
{
	return 1.0f - easeInElastic(1.0f - x);
}

UpgradeLayer::UpgradeLayer()
{
	Vector2 frame_grid{1, 1};
	m_card_texture_1 = PRELOAD_TEXTURE("card_1", "resources/ui/upgrade_card_1.png", frame_grid)->m_s_texture;
	m_card_texture_2 = PRELOAD_TEXTURE("card_2", "resources/ui/upgrade_card_2.png", frame_grid)->m_s_texture;
	m_card_texture_3 = PRELOAD_TEXTURE("card_3", "resources/ui/upgrade_card_3.png", frame_grid)->m_s_texture;
	m_button		 = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;
	m_gem_socket_texture = PRELOAD_TEXTURE("gem_socket", "resources/ui/gem_socket.png", frame_grid)->m_s_texture;
	m_tooltip_card_texture = PRELOAD_TEXTURE("tooltip_card", "resources/ui/tooltip_card.png", frame_grid)->m_s_texture;

	// Icons
	m_fire_rate_icon		= PRELOAD_TEXTURE("fire_rate_icon", "resources/icon/upgr_icon_firerate.png", frame_grid)->m_s_texture;
	m_luck_icon				= PRELOAD_TEXTURE("luck_icon", "resources/icon/upgr_icon_luck.png", frame_grid)->m_s_texture;
	m_projectile_speed_icon = PRELOAD_TEXTURE("projectile_speed_icon", "resources/icon/upgr_icon_projectile_speed.png", frame_grid)->m_s_texture;
	m_fire_range_icon		= PRELOAD_TEXTURE("fire_range_icon", "resources/icon/upgr_icon_projectile_range.png", frame_grid)->m_s_texture;
	m_add_cannon_icon		= PRELOAD_TEXTURE("add_cannon_icon", "resources/icon/upgr_icon_cannon_amount.png", frame_grid)->m_s_texture;
	m_turn_speed_icon		= PRELOAD_TEXTURE("turn_speed_icon", "resources/icon/upgr_icon_turn_speed.png", frame_grid)->m_s_texture;
	m_piercing_chance_icon	= PRELOAD_TEXTURE("piercing_chance_icon", "resources/icon/upgr_icon_piercing_chance.png", frame_grid)->m_s_texture;
	m_player_speed_icon		= PRELOAD_TEXTURE("player_speed_icon", "resources/icon/upgr_icon_movement_speed.png", frame_grid)->m_s_texture;
	m_explisve_barrel_icon	= PRELOAD_TEXTURE("explosive_barrel_icon", "resources/icon/upgr_icon_explosives.png", frame_grid)->m_s_texture;
	m_health_icon			= PRELOAD_TEXTURE("health_icon", "resources/icon/upgr_icon_health.png", frame_grid)->m_s_texture;
	m_projectile_amount_icon = PRELOAD_TEXTURE("projectile_amount_icon", "resources/icon/upgr_icon_multishot.png", frame_grid)->m_s_texture;

	m_card_1_texture_emissive =
			PRELOAD_TEXTURE("card_emissive_1", "resources/emissive/upgrate_card_emissive_border_and_center_card_1.png", frame_grid)->m_s_texture;
	m_card_2_texture_emissive =
			PRELOAD_TEXTURE("card_emissive_2", "resources/emissive/upgrate_card_emissive_border_and_center_card_2.png", frame_grid)->m_s_texture;
	m_card_3_texture_emissive =
			PRELOAD_TEXTURE("card_emissive_3", "resources/emissive/upgrate_card_emissive_border_and_center_card_3.png", frame_grid)->m_s_texture;

	m_emissive_texture_position = GetShaderLocation(m_card_emissive_shader, "texture_emissive");
	m_emissive_color_position	= GetShaderLocation(m_card_emissive_shader, "emissive_color");
	SetShaderValue(m_card_emissive_shader, m_emissive_color_position, &m_card_1_texture_emissive, SHADER_UNIFORM_SAMPLER2D);


	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());

	m_loot_table.add_loot_table(0, director->drop_chances.add_cannon, m_only_mythic_chance); // Add Cannon
	m_loot_table.add_loot_table(1, director->drop_chances.projectile_speed, m_chances); // Projectile Speed
	m_loot_table.add_loot_table(2, director->drop_chances.fire_range, m_chances); // Fire Range
	m_loot_table.add_loot_table(3, director->drop_chances.fire_rate, m_chances); // Fire Rate
	m_loot_table.add_loot_table(4, director->drop_chances.health, m_only_epic_chance); // Health
	m_loot_table.add_loot_table(5, director->drop_chances.speed, m_chances); // Player Speed
	m_loot_table.add_loot_table(6, director->drop_chances.rotation_speed, m_chances); // Turn Speed
	m_loot_table.add_loot_table(7, director->drop_chances.piercing_chance, m_chances); // Piercing Chance
	m_loot_table.add_loot_table(8, director->drop_chances.luck, m_chances); // Luck
	m_loot_table.add_loot_table(9, director->drop_chances.projectile_amount, m_only_mythic_chance); // Projectile Amount
	m_loot_table.add_loot_table(10, director->drop_chances.explosive_barrels, m_only_mythic_chance); // Explosive Barrels

	m_gem_sprite		  = PRELOAD_TEXTURE("gem_sprite", "resources/icon/gem_icon.png", frame_grid);
	m_gem_texture		  = m_gem_sprite->m_s_texture;
	m_gem_anim_controller = PSCore::sprites::SpriteSheetAnimation(
			m_gem_texture, {
								   {11, 0.1, PSCore::sprites::Forward, m_z_index},
						   }
	);
	m_gem_anim_controller.add_animation_at_index(0, m_z_index);
}

UpgradeLayer::~UpgradeLayer()
{
	UnloadShader(m_card_emissive_shader);
}

void UpgradeLayer::on_update(float dt)
{
	gApp()->show_custom_cursor();
	m_time_since_opened += dt;
	if ( m_time_since_opened > 0.5f ) {
		m_can_receive_input = true;
	}

	for ( int i = 0; i < 3; ++i ) {
		if ( m_time_since_opened > m_card_anim_delays[i] ) {
			m_card_anim_timers[i] += dt;
			if ( m_card_anim_timers[i] > m_card_anim_duration ) {
				m_card_anim_timers[i] = m_card_anim_duration;
			}
		}
	}

	if ( m_current_loot_table_values.size() >= 3 ) {
		auto& vp			  = gApp()->viewport();
		Vector2 origin		  = vp->viewport_origin();
		float scale			  = vp->viewport_scale();
		Vector2 screen_middel = {
				origin.x / vp->viewport_scale() + vp->viewport_base_size().x / 2, origin.y / vp->viewport_scale() + vp->viewport_base_size().y / 2
		};

		float card_offsets_x[]	  = {0.0f, static_cast<float>(m_card_texture_2.width + 16), static_cast<float>(-(m_card_texture_3.width + 16))};
		Texture2D card_textures[] = {m_card_texture_1, m_card_texture_2, m_card_texture_3};
		Vector2 mouse			  = GetMousePosition();

		bool card_hovered = false;
		for ( int i = 0; i < 3; ++i ) {
			float anim_t		= (m_card_anim_duration > 0.0f) ? (m_card_anim_timers[i] / m_card_anim_duration) : 1.0f;
			float anim_y_offset = m_card_anim_start_offset_y * (1.0f - easeOutElastic(anim_t));
			float cx			= (screen_middel.x + card_offsets_x[i]) * scale;
			float cy			= (screen_middel.y + anim_y_offset + m_card_hover_lift * m_card_hover_progress[i]) * scale;
			float hw			= card_textures[i].width / 2.0f * scale;
			float hh			= card_textures[i].height / 2.0f * scale;

			Rectangle card_rect = {cx - hw, cy - hh, hw * 2.0f, hh * 2.0f};
			bool hovered		= CheckCollisionPointRec(mouse, card_rect);

			float target = hovered ? 1.0f : 0.0f;
			m_card_hover_progress[i] += (target - m_card_hover_progress[i]) * m_card_hover_speed * dt;
			if ( m_card_hover_progress[i] < 0.001f )
				m_card_hover_progress[i] = 0.0f;
			if ( m_card_hover_progress[i] > 0.999f )
				m_card_hover_progress[i] = 1.0f;

			if ( hovered ) {
				m_hovered_card_index = i;
				m_card_hovered		 = true;
				card_hovered		 = true;
			}
		}
		if ( !card_hovered ) {
			m_hovered_card_index = -1;
			m_card_hovered		 = false;
		}
	}

	if ( m_reroll_anim_playing ) {
		play_reroll_gem_animation(dt);
	}
	
}

void UpgradeLayer::on_render()
{
	if ( m_layer_is_visible ) {
		draw_upgrade_cards();
		draw_reroll_button();
	}
}


void UpgradeLayer::draw_upgrade_cards()
{
	if ( m_current_loot_table_values.size() < 3 ) {
		return;
	}

	auto& vp			  = gApp()->viewport();
	Vector2 origin		  = vp->viewport_origin();
	float scale			  = vp->viewport_scale();
	Vector2 screen_middel = {
			origin.x / vp->viewport_scale() + vp->viewport_base_size().x / 2, origin.y / vp->viewport_scale() + vp->viewport_base_size().y / 2
	};

	Texture2D card_textures[]		   = {m_card_texture_1, m_card_texture_2, m_card_texture_3};
	Texture2D card_emissive_textures[] = {m_card_1_texture_emissive, m_card_2_texture_emissive, m_card_3_texture_emissive};
	float card_offsets_x[]			   = {0.0f, static_cast<float>(m_card_texture_2.width + 16), static_cast<float>(-(m_card_texture_3.width + 16))};

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));

	for ( int i = 0; i < 3; ++i ) {
		float anim_t		 = (m_card_anim_duration > 0.0f) ? (m_card_anim_timers[i] / m_card_anim_duration) : 1.0f;
		float anim_y_offset	 = m_card_anim_start_offset_y * (1.0f - easeOutElastic(anim_t));
		float hover_y_offset = m_card_hover_lift * m_card_hover_progress[i];
		Vector2 card_pos	 = {screen_middel.x + card_offsets_x[i], screen_middel.y - anim_y_offset + hover_y_offset};

		set_boarder_color(m_current_loot_table_values[i].rarity);
		BeginShaderMode(m_card_emissive_shader);
		SetShaderValueTexture(m_card_emissive_shader, m_emissive_texture_position, card_emissive_textures[i]);
		SetShaderValue(m_card_emissive_shader, m_emissive_color_position, &m_emissive_color, SHADER_UNIFORM_VEC3);

		if ( m_current_loot_table_values.at(i).rarity == 5 && m_can_play_mythic_sound ) {
			m_can_play_mythic_sound = false;
			gApp()->play_ui_sound(3);
		}

		float max_other_hover = 0.0f;
		for ( int j = 0; j < 3; ++j ) {
			if ( j != i ) max_other_hover = fmaxf(max_other_hover, m_card_hover_progress[j]);
		}
		float dim = 1.0f - max_other_hover * 0.5f;
		Color card_color = {
			(unsigned char)(255 * dim),
			(unsigned char)(255 * dim),
			(unsigned char)(255 * dim),
			255
		};

		if ( GuiButtonTexture(card_textures[i], card_pos, 0, scale, card_color, WHITE, "") && m_can_receive_input ) {
			apply_upgrade(m_current_loot_table_values[i]);
			gApp()->call_later([]() { gApp()->pop_layer<UpgradeLayer>(); });
			gApp()->call_later([]() {
				auto app_layer = gApp()->get_layer<AppLayer>();
				if ( app_layer )
					app_layer->resume();
				gApp()->hide_custom_cursor();
			});
		}
		EndShaderMode();

		Vector2 scaled_pos = {card_pos.x * scale, card_pos.y * scale};
		draw_upgrade_icon(m_current_loot_table_values[i].index, scaled_pos);
		draw_card_text(scaled_pos, m_current_loot_table_values[i]);
		m_current_tooltip_text = get_tooltip_text(m_current_loot_table_values[i].index);
		if ( m_hovered_card_index == i ) {
			draw_card_tooltip(card_pos, scale);
		}
	}
}

void UpgradeLayer::apply_upgrade(LootTableValue upgrade_info)
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());

	float upgrade_multyplier = get_multiplier(upgrade_info.rarity);
	float upgrade_amount;

	gApp()->play_ui_sound(2);

	switch ( upgrade_info.index ) {
		case 0:
			director->upgrade_player_add_cannon(m_base_upgrade_add_cannon);
			director->drop_chances.add_cannon = 0;
			PS_LOG(LOG_INFO, "Add Cannon Upgrade Applied");
			break;
		case 1:
			upgrade_amount = director->player_current_projectile_speed() * (m_base_upgrade_projectile_speed * upgrade_multyplier);
			director->upgrade_player_projectile_speed(upgrade_amount);
			PS_LOG(LOG_INFO, std::format("Projectile Speed Upgrade Applied: {:.2f}", upgrade_amount));
			break;
		case 2:
			upgrade_amount = director->player_current_fire_range() * (m_base_upgrade_fire_range * upgrade_multyplier);
			director->upgrade_player_fire_range(upgrade_amount);
			PS_LOG(LOG_INFO, std::format("Fire Range Upgrade Applied: {:.2f}", upgrade_amount));
			break;
		case 3:
			upgrade_amount = director->player_current_fire_rate() * (m_base_upgrade_fire_rate * upgrade_multyplier);
			director->upgrade_player_fire_rate(upgrade_amount);
			PS_LOG(LOG_INFO, std::format("Fire Rate Upgrade Applied: {:.2f}", upgrade_amount));
			break;
		case 4:
			director->upgrade_player_health(m_base_upgrade_health);
			PS_LOG(LOG_INFO, std::format("Health Upgrade Applied: {}", m_base_upgrade_health));
			break;
		case 5:
			upgrade_amount = director->player_max_velocity() * (m_base_upgrade_player_speed * upgrade_multyplier);
			director->upgrade_player_speed(upgrade_amount);
			PS_LOG(LOG_INFO, std::format("Player Speed Upgrade Applied: {:.2f}", upgrade_amount));
			break;
		case 6:
			upgrade_amount = director->player_input_rotation_mult() * (m_base_upgrade_rotation_speed * upgrade_multyplier);
			director->upgrade_player_rotation_speed(upgrade_amount);
			PS_LOG(LOG_INFO, std::format("Rotation Speed Upgrade Applied: {:.2f}", upgrade_amount));
			break;
		case 7:
			upgrade_amount = director->player_piercing_chance() * (m_base_upgrade_piercing_chance * upgrade_multyplier);
			director->upgrade_player_piercing_chance(upgrade_amount);
			PS_LOG(LOG_INFO, std::format("Piercing Chance Upgrade Applied: {:.2f}", upgrade_amount));
			PS_LOG(LOG_INFO, std::format("Player Piercing Chance: {:.2f}", director->player_piercing_chance()));
			break;
		case 8:
			upgrade_amount = director->player_luck() * (m_base_upgrade_luck * upgrade_multyplier);
			director->upgrade_player_luck(upgrade_amount);
			m_loot_table.set_expected_value(director->player_luck());
			PS_LOG(LOG_INFO, std::format("Luck Upgrade Applied: {:.2f}", upgrade_amount));
			PS_LOG(LOG_INFO, std::format("Player Luck: {:.2f}", director->player_luck()));
			break;
		case 9:
			director->upgrade_player_projectile_amount(1);
			if ( director->player_projectile_amount() == 3 ) {
				director->drop_chances.projectile_amount = 0;
			}
			PS_LOG(LOG_INFO, "Projectile Amount Upgrade Applied: +1 Projectile");
			break;
		case 10:
			director->upgrade_player_explosive_barrels();
			director->drop_chances.explosive_barrels = 0;
			PS_LOG(LOG_INFO, "Explosive Barrels Upgrade Applied");
			break;
		default:
			PS_LOG(LOG_WARNING, std::format("Invalid upgrade index: {}", upgrade_info.index));
	}
}

void UpgradeLayer::print_loot_table_values(std::vector<LootTableValue> values)
{
	for ( const auto& value: values ) {
		PS_LOG(LOG_INFO, std::format("Index: {} Value: {:.2f} Rarity: {}", value.index, value.value, value.rarity));
	}
}

void UpgradeLayer::draw_card_text(Vector2 card_pos, LootTableValue upgrade_info)
{
	auto& vp				  = gApp()->viewport();
	float scale				  = vp->viewport_scale();
	float text_size			  = 13 * scale;
	Rectangle rarity_text_pos = {card_pos.x + 10 * scale, card_pos.y - 80 * scale, 150 * scale, text_size};
	Rectangle value_text_pos  = {card_pos.x + 10 * scale, card_pos.y + 28 * scale + 2 * text_size, 150 * scale, text_size};
	Rectangle type_text_pos	  = {card_pos.x + 10 * scale, card_pos.y + 25 * scale + text_size, 150 * scale, text_size};
	float text_pos_x		  = card_pos.x - 75 * scale;
	rarity_text_pos.x		  = text_pos_x;
	type_text_pos.x			  = text_pos_x;
	value_text_pos.x		  = text_pos_x;


	GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

	float line_height = text_size + 4 * scale;

	GuiLabel(rarity_text_pos, rarity_to_string(upgrade_info.rarity).c_str());
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({219, 180, 132, 255}));
	GuiLabel(type_text_pos, upgrade_type_to_string(upgrade_info.index).c_str());
	GuiLabel(value_text_pos, value_to_string(upgrade_info.index, upgrade_info.rarity).c_str());

	draw_upgrade_preview(card_pos, upgrade_info);

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
}

std::string UpgradeLayer::rarity_to_string(int rarity)
{
	switch ( rarity ) {
		case 0:
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({60, 60, 60, 255}));
			return "Common";
		case 1:
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({84, 130, 53, 255}));
			return "Uncommon";
		case 2:
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0, 112, 192, 255}));
			return "Rare";
		case 3:
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({112, 48, 160, 255}));
			return "Epic";
		case 4:
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({255, 128, 0, 255}));
			return "Legendary";
		case 5:
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({255, 0, 0, 255}));
			return "Mythic";
		default:
			GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
			return "Unknown";
	}
}

void UpgradeLayer::set_boarder_color(int rarity)
{
	switch ( rarity ) {
		case 0:
			m_emissive_color = {128, 128, 128};
			break;
		case 1:
			m_emissive_color = {84, 130, 53};
			break;
		case 2:
			m_emissive_color = {0, 112, 192};
			break;
		case 3:
			m_emissive_color = {112, 48, 160};
			break;
		case 4:
			m_emissive_color = {255, 128, 0};
			break;
		case 5:
			m_emissive_color = {255, 0, 0};
			break;
		default:
			m_emissive_color = {255, 255, 255};
			break;
	}
}

std::string UpgradeLayer::upgrade_type_to_string(int index)
{
	switch ( index ) {
		case 0:
			return "Add Cannon";
		case 1:
			return "Projectile Speed";
		case 2:
			return "Fire Range";
		case 3:
			return "Fire Rate";
		case 4:
			return "Health";
		case 5:
			return "Player Speed";
		case 6:
			return "Turn Speed";
		case 7:
			return "Piercing Chance";
		case 8:
			return "Luck";
		case 9:
			return "Multi Shot";
		case 10:
			return "BOOOM!";
		default:
			return "Unknown";
	}
}

std::string UpgradeLayer::value_to_string(int index, int rarity)
{
	float multiplier = get_multiplier(rarity);
	float value		 = 0;
	switch ( index ) {
		case 0:
			return "+" + std::to_string(m_base_upgrade_add_cannon);
		case 1:
			return std::format("+{:.1f}%", m_base_upgrade_projectile_speed * multiplier * 100);
		case 2:
			return std::format("+{:.1f}%", m_base_upgrade_fire_range * multiplier * 100);
		case 3:
			return std::format("+{:.1f}%", m_base_upgrade_fire_rate * multiplier * 100);
		case 4:
			return "+" + std::to_string(m_base_upgrade_health);
		case 5:
			return std::format("+{:.1f}%", m_base_upgrade_player_speed * multiplier * 100);
		case 6:
			return std::format("+{:.1f}%", m_base_upgrade_rotation_speed * multiplier * 100);
		case 7:
			return std::format("+{:.1f}%", m_base_upgrade_piercing_chance * multiplier * 100);
		case 8:
			return std::format("+{:.1f}%", m_base_upgrade_luck * multiplier * 100);
		case 9:
			return "+" + std::to_string(m_base_upgrade_projectile_amount);
		case 10:
			return "";
		default:
			return "Unknown";
	}
}

float UpgradeLayer::get_multiplier(int rarity)
{
	switch ( rarity ) {
		case 0:
			return m_multiplier_common;
		case 1:
			return m_multiplier_uncommon;
		case 2:
			return m_multiplier_rare;
		case 3:
			return m_multiplier_epic;
		case 4:
			return m_multiplier_legendary;
		case 5:
			return m_multiplier_mythic;
		default:
			PS_LOG(LOG_WARNING, std::format("Invalid rarity index: {}", rarity));
			return 1.0f;
	}
}

void UpgradeLayer::draw_upgrade_preview(Vector2 card_pos, LootTableValue upgrade_info)
{
	auto director			 = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	float upgrade_multyplier = get_multiplier(upgrade_info.rarity);

	auto& vp		   = gApp()->viewport();
	float scale		   = vp->viewport_scale();
	float text_size	   = 10 * scale;
	Rectangle text_pos = {card_pos.x - 60 * scale, card_pos.y + 40 * scale + 3 * 14 * scale, 120 * scale, text_size};

	GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({219, 180, 132, 255}));
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

	std::string preview;

	switch ( upgrade_info.index ) {
		case 0:
			break;
		case 1:
			preview = std::format(
					"{:.1f} -> {:.1f}", director->player_current_projectile_speed(),
					director->player_current_projectile_speed() +
							director->player_current_projectile_speed() * (m_base_upgrade_projectile_speed * upgrade_multyplier)
			);
			break;
		case 2:
			preview = std::format(
					"{:.1f} -> {:.1f}", director->player_current_fire_range(),
					director->player_current_fire_range() + director->player_current_fire_range() * (m_base_upgrade_fire_range * upgrade_multyplier)
			);
			break;
		case 3:
			preview = std::format(
					"{:.3f}s -> {:.3f}s", director->player_current_fire_rate(),
					director->player_current_fire_rate() - director->player_current_fire_rate() * (m_base_upgrade_fire_rate * upgrade_multyplier)
			);
			break;
		case 4:
			break;
		case 5:
			preview = std::format(
					"{:.1f} -> {:.1f}", director->player_max_velocity(),
					director->player_max_velocity() + director->player_max_velocity() * (m_base_upgrade_player_speed * upgrade_multyplier)
			);
			break;
		case 6:
			preview = std::format(
					"{:.1f} -> {:.1f}", director->player_input_rotation_mult(),
					director->player_input_rotation_mult() +
							director->player_input_rotation_mult() * (m_base_upgrade_rotation_speed * upgrade_multyplier)
			);
			break;
		case 7:
			preview = std::format(
					"{:.1f}% -> {:.1f}%", director->player_piercing_chance(),
					(director->player_piercing_chance() + director->player_piercing_chance() * (m_base_upgrade_piercing_chance * upgrade_multyplier))
			);
			break;
		case 8:
			preview = std::format(
					"{:.1f}% -> {:.1f}%", director->player_luck() * 100,
					(director->player_luck() + director->player_luck() * (m_base_upgrade_luck * upgrade_multyplier)) * 100
			);
			break;
		case 9:
			preview = std::format(
					"{} -> {}", director->player_projectile_amount(), director->player_projectile_amount() + m_base_upgrade_projectile_amount
			);
			break;
		case 10:
			preview = "New Weapon Type";
			break;
		default:
			break;
	}

	if ( !preview.empty() ) {
		GuiLabel(text_pos, preview.c_str());
	}

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
}

void UpgradeLayer::draw_reroll_button()
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( director->reroll_amount() > 0 ) {
		auto& vp			  = gApp()->viewport();
		Vector2 origin		  = vp->viewport_origin();
		Vector2 screen_middel = {
				origin.x / vp->viewport_scale() + vp->viewport_base_size().x / 2, origin.y / vp->viewport_scale() + vp->viewport_base_size().y / 2
		};
		float scale = vp->viewport_scale();
		if ( GuiButtonTexture(m_button, {screen_middel.x, screen_middel.y + 130}, 0, scale, WHITE, GRAY, "Reroll") &&
			 m_can_receive_input ) {
			m_current_loot_table_values = m_loot_table.loot_table_values(3);
			director->set_reroll_amount(director->reroll_amount() - 1);
			gApp()->play_ui_sound(0);
			m_can_play_mythic_sound = true;
		}
		Vector2 mouse = GetMousePosition();

		Rectangle button_rect = {
				screen_middel.x * scale - (m_button.width * scale) / 2.0f, (screen_middel.y + 130) * scale - (m_button.height * scale) / 2.0f,
				m_button.width * scale, m_button.height * scale};

		bool hovered = CheckCollisionPointRec(mouse, button_rect);

		if ( hovered ) {
			m_reroll_anim_playing = true;
		} 
		else {
			m_reroll_anim_playing = false;
			m_gem_anim_controller.set_animation_at_index(0, 0, m_z_index);
		}

		Rectangle gem_source  = m_gem_anim_controller.get_source_rectangle(m_z_index).value_or(Rectangle{0});
		float button_center_x = screen_middel.x * scale;
		float button_center_y = (screen_middel.y + 130) * scale;
		float gem_center_x	  = button_center_x - (m_button.width / 2.0f - gem_source.width / 2.0f - 4.0f) * scale;


		Rectangle gem_socket_source = {0, 0, static_cast<float>(m_gem_socket_texture.width), static_cast<float>(m_gem_socket_texture.height)};
		Rectangle gem_socket_dest = {button_center_x, button_center_y + 27 * scale, gem_socket_source.width * scale, gem_socket_source.height * scale};
		Vector2 gem_socket_origin	= {gem_socket_dest.width / 2.0f, gem_socket_dest.height / 2.0f};

		Rectangle gem_dest = {gem_socket_dest.x - 15 * scale, gem_socket_dest.y, gem_source.width * scale, gem_source.height * scale};
		Vector2 gem_origin = {gem_dest.width / 2.0f, gem_dest.height / 2.0f};
		DrawTexturePro(m_gem_socket_texture, gem_socket_source, gem_socket_dest, gem_socket_origin, 0, WHITE);
		DrawTexturePro(m_gem_texture, gem_source, gem_dest, gem_origin, 0, WHITE);

		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({211, 177, 125, 255}));
		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
		GuiLabel({gem_socket_dest.x - 5 *scale , gem_socket_dest.y -10 * scale, 35 * scale, 20 * scale},
				std::format("{}x", director->reroll_amount()).c_str());
		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0, 0, 0, 255}));
	}
}

void UpgradeLayer::draw_upgrade_icon(int index, Vector2 card_pos)
{
	auto& vp	= gApp()->viewport();
	float scale = vp->viewport_scale();
	Vector2 pos = {card_pos.x - (m_fire_rate_icon.width * scale) / 2.0f, card_pos.y - 49 * scale};

	switch ( index ) {
		case 0:
			DrawTextureEx(m_add_cannon_icon, pos, 0, scale, WHITE);
			break;
		case 1:
			DrawTextureEx(m_projectile_speed_icon, pos, 0, scale, WHITE);
			break;
		case 2:
			DrawTextureEx(m_fire_range_icon, pos, 0, scale, WHITE);
			break;
		case 3:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 4:
			DrawTextureEx(m_health_icon, pos, 0, scale, WHITE);
			break;
		case 5:
			DrawTextureEx(m_player_speed_icon, pos, 0, scale, WHITE);
			break;
		case 6:
			DrawTextureEx(m_turn_speed_icon, pos, 0, scale, WHITE);
			break;
		case 7:
			DrawTextureEx(m_piercing_chance_icon, pos, 0, scale, WHITE);
			break;
		case 8:
			DrawTextureEx(m_luck_icon, pos, 0, scale, WHITE);
			break;
		case 9:
			DrawTextureEx(m_projectile_amount_icon, pos, 0, scale, WHITE);
			break;
		case 10:
			DrawTextureEx(m_explisve_barrel_icon, pos, 0, scale, WHITE);
			break;
		default:
			break;
	}
}

void UpgradeLayer::play_reroll_gem_animation(float dt)
{
	m_gem_anim_controller.update_animation(dt);
	if ( m_gem_anim_controller.get_sprite_sheet_frame_index(m_z_index).value_or(-1) == 10 ) {
		m_gem_anim_controller.set_animation_at_index(0, 0, m_z_index);
	}
}

void UpgradeLayer::draw_card_tooltip(Vector2 card_pos, float scale)
{
	float card_half_height	  = m_card_texture_1.height / 2.0f;
	float tooltip_half_height = m_tooltip_card_texture.height / 2.0f;

	Vector2 tooltip_pos = {card_pos.x, card_pos.y - card_half_height - (tooltip_half_height + 5)};

	Rectangle source = {0, 0, static_cast<float>(m_tooltip_card_texture.width), static_cast<float>(m_tooltip_card_texture.height)};

	if ( auto& vp = gApp()->viewport() ) {
		Vector2 origin = vp->viewport_origin();
		float s		   = vp->viewport_scale();

		Vector2 viewport_tooltip_pos = {tooltip_pos.x - origin.x / s, tooltip_pos.y - origin.y / s};

		vp->draw_in_viewport(m_tooltip_card_texture, source, viewport_tooltip_pos, 0, WHITE);

		float label_w = source.width * s;
		float label_h = source.height * s;
		float label_x = tooltip_pos.x * s - label_w / 2.0f;
		float label_y = tooltip_pos.y * s - label_h / 2.0f;

		float text_padding_x = 8.0f * s;
		float text_offset_y	 = 10.0f * s;
		float text_padding_y = 4.0f * s;
		GuiSetStyle(DEFAULT, TEXT_SIZE, 6 * s);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0, 0, 0, 255}));
		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		GuiSetStyle(DEFAULT, TEXT_LINE_SPACING, 7 * s);
		GuiLabel(
				{label_x + text_padding_x, label_y + text_offset_y, label_w - 2 * text_padding_x, label_h - text_offset_y - text_padding_y},
				m_current_tooltip_text.c_str()
		);
		GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * s);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	}
}

std::string UpgradeLayer::get_tooltip_text(int upgrade_index)
{
	switch ( upgrade_index ) {
		case 0:
			return "Obtain one more canon for \neach side of your ship.";
		case 1:
			return "Increases the speed of \nprojectiles you shoot.";
		case 2:
			return "Increases the maximum \ndistance your projectiles can \ntravel.";
		case 3:
			return "Upgrades your shooting \nfrequency.";
		case 4:
			return "Gain one more health.";
		case 5:
			return "Increases the maximum \nship speed.";
		case 6:
			return "Increases your ship`s \nmaximum turn speed.";
		case 7:
			return "Bullets get an increased \nchance of piercing through \nenemies.";
		case 8:
			return "Increases the chance for \nrolling higher rarities when \nobtaining upgrades.";
		case 9:
			return "Each cannon shoots one more \nbullet.";
		case 10:
			return "Periodically drop explosive \nbarrels behind you that deal \nAoE damage when detonating.";
		default:
			return "";
	}
}