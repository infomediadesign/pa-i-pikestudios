#pragma once
#include <psinterfaces/layer.h>
#include "raylib.h"
#include "misc/loottable.h"

class UpgradeLayer : public PSInterfaces::Layer
{
public:

	UpgradeLayer();

	void on_update(float dt) override;

	void on_render() override;

	void apply_upgrade(LootTableValue upgrdae_info);

	void draw_upgrade_cards();

	void print_loot_table_values(std::vector<LootTableValue>);

	void draw_card_text(Vector2 card_pos, LootTableValue upgrade_info);

	float get_multiplier(int rarity);

	std::string rarity_to_string(int rarity);
	std::string upgrade_type_to_string(int index);
	std::string value_to_string(int index, int rarity);

	std::vector<int> m_chances{50, 30, 15, 4, 1};
	Texture2D m_card_texture;
	LootTable m_loot_table;
	std::vector<LootTableValue> m_current_loot_table_values;

	// base upgrade values for the different upgrades
	float m_base_upgrade_player_speed	= 0.075f;
	float m_base_upgrade_rotation_speed	  = 0.075f;
	float m_base_upgrade_projectile_speed = 0.1f;
	float m_base_upgrade_fire_range		  = 0.1f;
	float m_base_upgrade_fire_rate		  = 0.075f;
	int m_base_upgrade_health			  = 1;
	int m_base_upgrade_add_cannon		  = 1;


	// multipliers for the different rarities
	float m_multiplier_common = 1.0f;
	float m_multiplier_uncommon = 1.2f;
	float m_multiplier_rare		= 1.4f;
	float m_multiplier_epic = 1.6f;
	float m_multiplier_legendary = 2.0f;

};
