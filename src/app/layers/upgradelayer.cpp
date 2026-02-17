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
	auto director  = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	
	m_loot_table.add_loot_table(0,director->drop_chances.add_cannon, m_chances); //Add Cannon
	m_loot_table.add_loot_table(1,director->drop_chances.projectile_speed, m_chances); //Projectile Speed
	m_loot_table.add_loot_table(2,director->drop_chances.fire_range , m_chances); //Fire Range
	m_loot_table.add_loot_table(3,director->drop_chances.fire_rate, m_chances); //Fire Rate
	m_loot_table.add_loot_table(4,director->drop_chances.health, m_chances); //Health
	m_loot_table.add_loot_table(5,director->drop_chances.speed, m_chances); //Player Speed
	m_loot_table.add_loot_table(6,director->drop_chances.rotation_speed, m_chances); //Turn Speed
}

void UpgradeLayer::on_update(float dt)
{
}

void UpgradeLayer::on_render()
{
	draw_upgrade_cards();
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
	
	
	if ( GuiButtonTexture(m_card_texture, screen_middel, 0, scale, WHITE, GRAY, std::to_string(m_current_loot_table_values[0].index).c_str()) ) {
		apply_upgrade(m_current_loot_table_values[0]);
		gApp()->call_later([]() {
			gApp()->pop_layer<UpgradeLayer>(); });
	}
	draw_card_text(card_pos * scale, m_current_loot_table_values[0]);
	
	if ( GuiButtonTexture(
				 m_card_texture, {screen_middel.x + m_card_texture.width + 16, screen_middel.y}, 0, scale, WHITE, GRAY,
				 std::to_string(m_current_loot_table_values[1].index).c_str()))
		{
		apply_upgrade(m_current_loot_table_values[1]);
		gApp()->call_later([]() { gApp()->pop_layer<UpgradeLayer>(); });
	}

	if ( GuiButtonTexture(
				 m_card_texture, {screen_middel.x - m_card_texture.width - 16, screen_middel.y}, 0, scale, WHITE, GRAY,
				 std::to_string(m_current_loot_table_values[2].index).c_str()))
		{
		apply_upgrade(m_current_loot_table_values[2]);
		gApp()->call_later([]() { gApp()->pop_layer<UpgradeLayer>(); });
	}

}

void UpgradeLayer::apply_upgrade(LootTableValue upgrade_info)
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());

	float upgrade_multyplier = 1.0f;

	switch ( upgrade_info.rarity ) {

		case 0:
			upgrade_multyplier = m_multiplier_common;
			break;
		case 1:
			upgrade_multyplier = m_multiplier_uncommon;
			break;
		case 2:
			upgrade_multyplier = m_multiplier_rare;
			break;
		case 3:
			upgrade_multyplier = m_multiplier_epic;
			break;
		case 4:
			upgrade_multyplier = m_multiplier_legendary;
			break;
		default:
			std::cout << "Invalid rarity index: " << upgrade_info.rarity << std::endl;

	}

	float upgrade_amount;

	switch ( upgrade_info.index ) {
		case 0:
			director->upgrade_player_add_cannon(1);
			director->drop_chances.add_cannon = 0;
			printf("Upgrade applied: Added Cannon\n");
			break;
		case 1:
			upgrade_amount = director->player_current_projectile_speed() * (0.2f * upgrade_multyplier);
			director->upgrade_player_projectile_speed(upgrade_amount);
			printf("Upgrade applied: Projectile Speed increased by %f\n", 20.0f * upgrade_multyplier);
			printf("Current Projectile Speed: %f\n", director->player_current_projectile_speed());
			break;
		case 2:
			upgrade_amount = director->player_current_fire_range() * (0.2f * upgrade_multyplier);
			director->upgrade_player_fire_range(20.0f * upgrade_multyplier);
			printf("Upgrade applied: Fire Range increased by %f\n", 20.0f * upgrade_multyplier);
			printf("Current Fire Range: %f\n", director->player_current_fire_range());
			break;
		case 3:
			upgrade_amount = director->player_current_fire_rate() * (0.1f * upgrade_multyplier);
			director->upgrade_player_fire_rate(0.1f * upgrade_multyplier);
			printf("Upgrade applied: Fire Rate increased by %f\n", 0.1f * upgrade_multyplier);
			printf("Current Fire Rate: %f\n", director->player_current_fire_rate());
			break;
		case 4:
			director->upgrade_player_health(1);
			printf("Upgrade applied: Health increased by 1\n");
			printf("Current Health: %d / %d\n", director->player_health(), director->player_max_health());
			break;
		case 5:
			upgrade_amount = director->player_max_velocity() * (0.1f * upgrade_multyplier);
			director->upgrade_player_speed(upgrade_amount);
			printf("Upgrade applied: Speed increased by %f\n", upgrade_amount);
			printf("Current Speed: %f\n", director->player_max_velocity());
			break;
		case 6:
			upgrade_amount = director->player_input_rotation_mult() * (0.1f * upgrade_multyplier);
			director->upgrade_player_rotation_speed(upgrade_amount);
			printf("Upgrade applied: Rotation Speed increased by %f\n", upgrade_amount);
			printf("Current Rotation Speed: %f\n", director->player_input_rotation_mult());
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
	Rectangle rarity_text_pos = {card_pos.x + 10 * scale, card_pos.y - 60 * scale, 50 * scale, text_size};
	float text_pos_x		  = card_pos.x - 25 * scale;
	rarity_text_pos.x		  = text_pos_x;
	

	GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	
	float line_height = text_size + 4 * scale;

	GuiLabel(rarity_text_pos, rarity_to_string(upgrade_info.rarity).c_str());
	GuiLabel({card_pos.x + 10 * scale, card_pos.y + 10 * scale + line_height, 200 * scale, text_size}, upgrade_type_to_string(upgrade_info.index).c_str());
	GuiLabel(
			{card_pos.x + 10 * scale, card_pos.y + 10 * scale + line_height * 2, 200 * scale, text_size}, value_to_string(upgrade_info.index, 0).c_str()
	);

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * scale);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
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
		default:
			printf("Invalid rarity index: %d\n", rarity);
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
		default:
			return "Unknown";
	}
}

std::string UpgradeLayer::value_to_string(int index, float value)
{
	switch ( index ) {
		case 0:
			return "+1 Cannon";
		case 1:
			return "+" + std::to_string(static_cast<int>(value * 100)) + "% Projectile Speed";
		case 2:
			return "+" + std::to_string(static_cast<int>(value * 100)) + "% Fire Range";
		case 3:
			return "+" + std::to_string(static_cast<int>(value * 100)) + "% Fire Rate";
		case 4:
			return "+1 Health";
		case 5:
			return "+" + std::to_string(static_cast<int>(value * 100)) + "% Player Speed";
		case 6:
			return "+" + std::to_string(static_cast<int>(value * 100)) + "% Turn Speed";
		default:
			return "Unknown";
	}
}
