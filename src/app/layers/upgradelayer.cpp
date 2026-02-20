#include "layers/upgradelayer.h"
#include <iostream>
#include "pscore/sprite.h"
#include "pscore/viewport.h"
#include <raylib.h>
#include "raygui.h"
#include <string>
#include <entities/director.h>

UpgradeLayer::UpgradeLayer()
{
	Vector2 frame_grid{1, 1};
	m_card_texture = PRELOAD_TEXTURE("card", "resources/ui/test_upgrade_card.png", frame_grid)->m_s_texture;
	m_button	   = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;
	auto director  = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	
	m_loot_table.add_loot_table(0, director->drop_chances.add_cannon, m_only_mithic_chance); // Add Cannon
	m_loot_table.add_loot_table(1,director->drop_chances.projectile_speed, m_chances); //Projectile Speed
	m_loot_table.add_loot_table(2,director->drop_chances.fire_range , m_chances); //Fire Range
	m_loot_table.add_loot_table(3,director->drop_chances.fire_rate, m_chances); //Fire Rate
	m_loot_table.add_loot_table(4, director->drop_chances.health, m_only_epic_chance); // Health
	m_loot_table.add_loot_table(5,director->drop_chances.speed, m_chances); //Player Speed
	m_loot_table.add_loot_table(6,director->drop_chances.rotation_speed, m_chances); //Turn Speed
	m_loot_table.add_loot_table(7, director->drop_chances.piercing_chance, m_chances); // Piercing Chance
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
	auto& vp = gApp()->viewport();
	Vector2 origin = vp->viewport_origin();
	Vector2 screen_middel = {
			origin.x / vp->viewport_scale() + vp->viewport_base_size().x / 2, origin.y / vp->viewport_scale() + vp->viewport_base_size().y / 2
	};
	float scale	   = vp->viewport_scale();
	Vector2 card_pos	  = screen_middel;

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	
	
	if ( GuiButtonTexture(m_card_texture, screen_middel, 0, scale, WHITE, GRAY, "") && m_can_receive_input ) {
		apply_upgrade(m_current_loot_table_values[0]);
		gApp()->call_later([]() {
			gApp()->pop_layer<UpgradeLayer>(); });
		gApp()->call_later([]() {
			auto app_layer = gApp()->get_layer<AppLayer>();
			if ( app_layer )
					app_layer->resume();
		});
	}
	draw_card_text(card_pos * scale, m_current_loot_table_values[0]);
	
	if ( GuiButtonTexture(m_card_texture, {screen_middel.x + m_card_texture.width + 16, screen_middel.y}, 0, scale, WHITE, GRAY, "") &&
		 m_can_receive_input )
		{
		apply_upgrade(m_current_loot_table_values[1]);
		gApp()->call_later([]() { gApp()->pop_layer<UpgradeLayer>(); });
		gApp()->call_later([]() {
			auto app_layer = gApp()->get_layer<AppLayer>();
			if ( app_layer )
				app_layer->resume();
		});
	}

	draw_card_text({(screen_middel.x + m_card_texture.width + 16) * scale, screen_middel.y * scale}, m_current_loot_table_values[1]);

	if ( GuiButtonTexture(m_card_texture, {screen_middel.x - m_card_texture.width - 16, screen_middel.y}, 0, scale, WHITE, GRAY, "") &&
		 m_can_receive_input )
		{
		apply_upgrade(m_current_loot_table_values[2]);
		gApp()->call_later([]() { gApp()->pop_layer<UpgradeLayer>(); });
		gApp()->call_later([]() {
			auto app_layer = gApp()->get_layer<AppLayer>();
			if ( app_layer )
				app_layer->resume();
		});
	}

	draw_card_text({(screen_middel.x - m_card_texture.width - 16) * scale, screen_middel.y * scale}, m_current_loot_table_values[2]);

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
	auto& vp = gApp()->viewport();
	float scale = vp->viewport_scale();
	float text_size = 14 * scale;
	Rectangle rarity_text_pos = {card_pos.x + 10 * scale, card_pos.y - 60 * scale, 120 * scale, text_size};
	Rectangle value_text_pos  = {card_pos.x + 10 * scale, card_pos.y + 10 * scale + 2 * text_size, 120 * scale, text_size};
	Rectangle type_text_pos	  = {card_pos.x + 10 * scale, card_pos.y + 10 * scale + text_size, 120 * scale, text_size};
	float text_pos_x		  = card_pos.x - 60 * scale;
	rarity_text_pos.x		  = text_pos_x;
	type_text_pos.x			  = text_pos_x;
	value_text_pos.x		  = text_pos_x;
	

	GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	
	float line_height = text_size + 4 * scale;

	GuiLabel(rarity_text_pos, rarity_to_string(upgrade_info.rarity).c_str());
	GuiLabel(type_text_pos, upgrade_type_to_string(upgrade_info.index).c_str());
	GuiLabel(value_text_pos, value_to_string(upgrade_info.index, upgrade_info.rarity).c_str()
	);

	draw_upgrade_preview(card_pos, upgrade_info);

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
}

std::string UpgradeLayer::rarity_to_string(int rarity)
{
	switch ( rarity ) {
		case 0:
			return "Common";
		case 1:
			return "Uncommon";
		case 2:
			return "Rare";
		case 3:
			return "Epic";
		case 4:
			return "Legendary";
		case 5:
			return "Mythic";
		default:
			return "Unknown";
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
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	float upgrade_multyplier = get_multiplier(upgrade_info.rarity);

	auto& vp				  = gApp()->viewport();
	float scale				  = vp->viewport_scale();
	float text_size			  = 10 * scale;
	Rectangle text_pos		  = {card_pos.x - 60 * scale, card_pos.y + 30 * scale + 3 * 14 * scale, 120 * scale, text_size};

	GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

	std::string preview;

	switch ( upgrade_info.index ) {
		case 0:
			break;
		case 1:
			preview = std::format("{:.1f} -> {:.1f}",
				director->player_current_projectile_speed(),
				director->player_current_projectile_speed() + director->player_current_projectile_speed() * (m_base_upgrade_projectile_speed * upgrade_multyplier));
			break;
		case 2:
			preview = std::format("{:.1f} -> {:.1f}",
				director->player_current_fire_range(),
				director->player_current_fire_range() + director->player_current_fire_range() * (m_base_upgrade_fire_range * upgrade_multyplier));
			break;
		case 3:
			preview = std::format("{:.3f}s -> {:.3f}s",
				director->player_current_fire_rate(),
				director->player_current_fire_rate() - director->player_current_fire_rate() * (m_base_upgrade_fire_rate * upgrade_multyplier));
			break;
		case 4:
			break;
		case 5:
			preview = std::format("{:.1f} -> {:.1f}",
				director->player_max_velocity(),
				director->player_max_velocity() + director->player_max_velocity() * (m_base_upgrade_player_speed * upgrade_multyplier));
			break;
		case 6:
			preview = std::format("{:.1f} -> {:.1f}",
				director->player_input_rotation_mult(),
				director->player_input_rotation_mult() + director->player_input_rotation_mult() * (m_base_upgrade_rotation_speed * upgrade_multyplier));
			break;
		case 7:
			preview = std::format("{:.1f}% -> {:.1f}%",
				director->player_piercing_chance(),
				(director->player_piercing_chance() + director->player_piercing_chance() * (m_base_upgrade_piercing_chance * upgrade_multyplier)));
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
	if ( director->reroll_amount() > 0 ){
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
