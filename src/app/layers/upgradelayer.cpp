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
	
	m_loot_table.add_loot_table(0, m_chances);
	m_loot_table.add_loot_table(1, m_chances);
	m_loot_table.add_loot_table(2, m_chances);
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
	
	if ( GuiButtonTexture(m_card_texture, screen_middel, 0, scale, WHITE, GRAY, std::to_string(m_current_loot_table_values[0].index).c_str()) ) {
		apply_upgrade(m_current_loot_table_values[0].index);
		gApp()->call_later([]() {
			gApp()->pop_layer<UpgradeLayer>(); });
	}
	
	if ( GuiButtonTexture(
				 m_card_texture, {screen_middel.x + m_card_texture.width + 16, screen_middel.y}, 0, scale, WHITE, GRAY,
				 std::to_string(m_current_loot_table_values[1].index).c_str()))
		{
		apply_upgrade(m_current_loot_table_values[1].index);
		gApp()->call_later([]() { gApp()->pop_layer<UpgradeLayer>(); });
	}

	if ( GuiButtonTexture(
				 m_card_texture, {screen_middel.x - m_card_texture.width - 16, screen_middel.y}, 0, scale, WHITE, GRAY,
				 std::to_string(m_current_loot_table_values[1].index).c_str()))
		{
		apply_upgrade(m_current_loot_table_values[1].index);
		gApp()->call_later([]() { gApp()->pop_layer<UpgradeLayer>(); });
	}

}

void UpgradeLayer::apply_upgrade(int upgrade_index)
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());

	switch ( upgrade_index ) {
		case 0:
			director->upgrade_player_add_cannon(1);
			break;
		case 1:
			director->upgrade_player_projectile_speed(20.0f);
			break;
		case 2:
			director->upgrade_player_fire_range(20.0f);
			break;
		case 3:
			director->upgrade_player_fire_rate(0.1f);
			break;
		case 4:
			director->upgrade_player_invincibility(true);
			break;
		default:
			std::cout << "Invalid upgrade index: " << upgrade_index << std::endl;
	}
}

void UpgradeLayer::print_loot_table_values(std::vector<LootTableValue> values)
{
	for ( const auto& value: values ) {
		std::cout << "Index: " << value.index << " Value: " << value.value << " Rarity: " << value.rarity << std::endl;
	}
}
