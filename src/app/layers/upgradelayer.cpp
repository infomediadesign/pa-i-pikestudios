#include "layers/upgradelayer.h"
#include <entities/director.h>
#include <iostream>
#include <raylib.h>
#include <string>
#include "pscore/sprite.h"
#include "pscore/viewport.h"
#include "raygui.h"

UpgradeLayer::UpgradeLayer()
{
	Vector2 frame_grid{1, 1};
	m_card_texture_1 = PRELOAD_TEXTURE("card_1", "resources/ui/upgrade_card_1.png", frame_grid)->m_s_texture;
	m_card_texture_2 = PRELOAD_TEXTURE("card_2", "resources/ui/upgrade_card_2.png", frame_grid)->m_s_texture;
	m_card_texture_3 = PRELOAD_TEXTURE("card_3", "resources/ui/upgrade_card_3.png", frame_grid)->m_s_texture;
	m_button		 = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;

	// Icons
	m_fire_rate_icon = PRELOAD_TEXTURE("fire_rate_icon", "resources/icon/upgr_icon_firerate.png", frame_grid)->m_s_texture;
	m_luck_icon		 = PRELOAD_TEXTURE("luck_icon", "resources/icon/upgr_icon_luck.png", frame_grid)->m_s_texture;


	m_card_texture_emissive =
			PRELOAD_TEXTURE("card_emissive", "resources/emissive/upgrate_card_emissive_border_and_center_card_1.png", frame_grid)->m_s_texture;
	m_emissive_texture_position = GetShaderLocation(m_card_emissive_shader, "texture_emissive");
	m_emissive_color_position	= GetShaderLocation(m_card_emissive_shader, "emissive_color");
	SetShaderValue(m_card_emissive_shader, m_emissive_color_position, &m_card_texture_emissive, SHADER_UNIFORM_SAMPLER2D);


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
}

UpgradeLayer::~UpgradeLayer()
{
	UnloadShader(m_card_emissive_shader);
}

void UpgradeLayer::on_update(float dt)
{
	m_time_since_opened += dt;
	if ( m_time_since_opened > 0.5f ) {
		m_can_receive_input = true;
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
	auto& vp			  = gApp()->viewport();
	Vector2 origin		  = vp->viewport_origin();
	float scale			  = vp->viewport_scale();
	Vector2 screen_middel = {
			origin.x / vp->viewport_scale() + vp->viewport_base_size().x / 2, origin.y / vp->viewport_scale() + vp->viewport_base_size().y / 2
	};

	Texture2D card_textures[] = {m_card_texture_1, m_card_texture_2, m_card_texture_3};
	float card_offsets_x[]	  = {0.0f, static_cast<float>(m_card_texture_2.width + 16), static_cast<float>(-(m_card_texture_3.width + 16))};

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));

	for ( int i = 0; i < 3; ++i ) {
		Vector2 card_pos = {screen_middel.x + card_offsets_x[i], screen_middel.y};

		set_boarder_color(m_current_loot_table_values[i].rarity);
		BeginShaderMode(m_card_emissive_shader);
		SetShaderValueTexture(m_card_emissive_shader, m_emissive_texture_position, m_card_texture_emissive);
		SetShaderValue(m_card_emissive_shader, m_emissive_color_position, &m_emissive_color, SHADER_UNIFORM_VEC3);

		if ( GuiButtonTexture(card_textures[i], card_pos, 0, scale, WHITE, GRAY, "") && m_can_receive_input ) {
			apply_upgrade(m_current_loot_table_values[i]);
			gApp()->call_later([]() { gApp()->pop_layer<UpgradeLayer>(); });
			gApp()->call_later([]() {
				auto app_layer = gApp()->get_layer<AppLayer>();
				if ( app_layer )
					app_layer->resume();
			});
		}
		EndShaderMode();

		Vector2 scaled_pos = {card_pos.x * scale, card_pos.y * scale};
		draw_upgrade_icon(m_current_loot_table_values[i].index, scaled_pos);
		draw_card_text(scaled_pos, m_current_loot_table_values[i]);
	}
}

void UpgradeLayer::apply_upgrade(LootTableValue upgrade_info)
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());

	float upgrade_multyplier = get_multiplier(upgrade_info.rarity);
	float upgrade_amount;

	switch ( upgrade_info.index ) {
		case 0:
			director->upgrade_player_add_cannon(m_base_upgrade_add_cannon);
			director->drop_chances.add_cannon = 0;
			printf("Add Cannon Upgrade Applied\n");
			break;
		case 1:
			upgrade_amount = director->player_current_projectile_speed() * (m_base_upgrade_projectile_speed * upgrade_multyplier);
			director->upgrade_player_projectile_speed(upgrade_amount);
			printf("Projectile Speed Upgrade Applied: %.2f\n", upgrade_amount);
			break;
		case 2:
			upgrade_amount = director->player_current_fire_range() * (m_base_upgrade_fire_range * upgrade_multyplier);
			director->upgrade_player_fire_range(upgrade_amount);
			printf("Fire Range Upgrade Applied: %.2f\n", upgrade_amount);
			break;
		case 3:
			upgrade_amount = director->player_current_fire_rate() * (m_base_upgrade_fire_rate * upgrade_multyplier);
			director->upgrade_player_fire_rate(upgrade_amount);
			printf("Fire Rate Upgrade Applied: %.2f\n", upgrade_amount);
			break;
		case 4:
			director->upgrade_player_health(m_base_upgrade_health);
			printf("Health Upgrade Applied: %d\n", m_base_upgrade_health);
			break;
		case 5:
			upgrade_amount = director->player_max_velocity() * (m_base_upgrade_player_speed * upgrade_multyplier);
			director->upgrade_player_speed(upgrade_amount);
			printf("Player Speed Upgrade Applied: %.2f\n", upgrade_amount);
			break;
		case 6:
			upgrade_amount = director->player_input_rotation_mult() * (m_base_upgrade_rotation_speed * upgrade_multyplier);
			director->upgrade_player_rotation_speed(upgrade_amount);
			printf("Rotation Speed Upgrade Applied: %.2f\n", upgrade_amount);
			break;
		case 7:
			upgrade_amount = director->player_piercing_chance() * (m_base_upgrade_piercing_chance * upgrade_multyplier);
			director->upgrade_player_piercing_chance(upgrade_amount);
			printf("Piercing Chance Upgrade Applied: %.2f\n", upgrade_amount);
			printf("Player Piercing Chance: %.2f\n", director->player_piercing_chance());
			break;
		case 8:
			upgrade_amount = director->player_luck() * (m_base_upgrade_luck * upgrade_multyplier);
			director->upgrade_player_luck(upgrade_amount);
			m_loot_table.set_expected_value(director->player_luck());
			if ( director->player_luck() >= 1.0 ) {
				director->drop_chances.luck = 0;
			}
			printf("Luck Upgrade Applied: %.2f\n", upgrade_amount);
			printf("Player Luck: %.2f\n", director->player_luck());
			break;
		default:
			std::cout << "Invalid upgrade index: " << upgrade_info.index << std::endl;
	}
}

void UpgradeLayer::print_loot_table_values(std::vector<LootTableValue> values)
{
	for ( const auto& value: values ) {
		std::cout << "Index: " << value.index << " Value: " << value.value << " Rarity: " << value.rarity << std::endl;
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
			std::cout << "Invalid rarity index: " << rarity << std::endl;
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
					std::clamp(
							(director->player_luck() + director->player_luck() * (m_base_upgrade_luck * upgrade_multyplier)) * 100, -100.0f, 100.0f
					)
			);
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
		if ( GuiButtonTexture(
					 m_button, {screen_middel.x, screen_middel.y + 130}, 0, scale, WHITE, GRAY,
					 std::format("{}x Reroll", director->reroll_amount()).c_str()
			 ) &&
			 m_can_receive_input ) {
			m_current_loot_table_values = m_loot_table.loot_table_values(3);
			director->set_reroll_amount(director->reroll_amount() - 1);
		}
	}
}

void UpgradeLayer::draw_upgrade_icon(int index, Vector2 card_pos)
{
	auto& vp	= gApp()->viewport();
	float scale = vp->viewport_scale();
	Vector2 pos = {card_pos.x - (m_fire_rate_icon.width * scale) / 2.0f, card_pos.y - 49 * scale};

	switch ( index ) {
		case 0:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 1:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 2:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 3:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 4:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 5:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 6:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 7:
			DrawTextureEx(m_fire_rate_icon, pos, 0, scale, WHITE);
			break;
		case 8:
			DrawTextureEx(m_luck_icon, pos, 0, scale, WHITE);
			break;
		default:
			break;
	}
}
