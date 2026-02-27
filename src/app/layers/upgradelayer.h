#pragma once
#include <pscore/settings.h>
#include <psinterfaces/layer.h>
#include "misc/loottable.h"
#include "raylib.h"

class UpgradeLayer : public PSInterfaces::Layer
{
public:
	UpgradeLayer();
	~UpgradeLayer();

	void on_update(float dt) override;

	void on_render() override;

	void apply_upgrade(LootTableValue upgrdae_info);

	void draw_upgrade_cards();

	void print_loot_table_values(std::vector<LootTableValue>);

	void draw_card_text(Vector2 card_pos, LootTableValue upgrade_info);

	void draw_reroll_button();

	void draw_upgrade_preview(Vector2 card_pos, LootTableValue upgrade_info);

	void set_boarder_color(int rarity);

	float get_multiplier(int rarity);

	std::string rarity_to_string(int rarity);
	std::string upgrade_type_to_string(int index);
	std::string value_to_string(int index, int rarity);
	void draw_upgrade_icon(int index, Vector2 pos);

	float common_pull_chance	= CFG_VALUE<float>("upgrade_common_chance", 50);
	float uncommon_pull_chance	= CFG_VALUE<float>("upgrade_uncommon_chance", 30);
	float rare_pull_chance		= CFG_VALUE<float>("upgrade_rare_chance", 15);
	float epic_pull_chance		= CFG_VALUE<float>("upgrade_epic_chance", 4.5);
	float legendary_pull_chance = CFG_VALUE<float>("upgrade_legendary_chance", 0.5);
	float mythic_pull_chance	= CFG_VALUE<float>("upgrade_mythical_chance", 0);

	std::vector<float> m_chances{common_pull_chance, uncommon_pull_chance,	rare_pull_chance,
								 epic_pull_chance,	 legendary_pull_chance, mythic_pull_chance};

	std::vector<float> m_only_mythic_chance{0, 0, 0, 0, 0, 100};
	std::vector<float> m_only_epic_chance{0, 0, 0, 100, 0, 0};

	Texture2D m_card_texture_1;
	Texture2D m_card_texture_2;
	Texture2D m_card_texture_3;

	Texture2D m_card_1_texture_emissive;
	Texture2D m_card_2_texture_emissive;
	Texture2D m_card_3_texture_emissive;

	Shader m_card_emissive_shader = LoadShader(NULL, "resources/shader/emissive_color.fs");
	Vector3 m_emissive_color{255, 0, 0};
	int m_emissive_texture_position;
	int m_emissive_color_position;

	LootTable m_loot_table;
	std::vector<LootTableValue> m_current_loot_table_values;

	bool m_layer_is_visible = true;
	Texture2D m_button;
	bool m_can_receive_input  = false;
	float m_time_since_opened = 0;

	// Upgrade Icons
	Texture2D m_fire_rate_icon;
	Texture2D m_luck_icon;
	Texture2D m_projectile_speed_icon;
	Texture2D m_fire_range_icon;
	Texture2D m_add_cannon_icon;
	Texture2D m_turn_speed_icon;

	// base upgrade values for the different upgrades
	float m_base_upgrade_player_speed	  = CFG_VALUE<float>("upgrade_player_speed_base_value", 0.075f);
	float m_base_upgrade_rotation_speed	  = CFG_VALUE<float>("upgrade_player_rotation_speed_base_value", 0.075f);
	float m_base_upgrade_projectile_speed = CFG_VALUE<float>("upgrade_projectile_speed_base_value", 0.1f);
	float m_base_upgrade_fire_range		  = CFG_VALUE<float>("upgrade_fire_range_base_value", 0.1f);
	float m_base_upgrade_fire_rate		  = CFG_VALUE<float>("upgrade_fire_rate_base_value", 0.075f);
	float m_base_upgrade_piercing_chance  = CFG_VALUE<float>("upgrade_piercing_chance_base_value", 0.05f);
	float m_base_upgrade_loot_drop_chance = CFG_VALUE<float>("upgrade_loot_drop_chance_base_value", 0.05);
	float m_base_upgrade_luck			  = CFG_VALUE<float>("upgrade_luck_base_value", 0.075);
	int m_base_upgrade_health			  = CFG_VALUE<int>("upgrade_health_base_value", 1);
	int m_base_upgrade_add_cannon		  = CFG_VALUE<int>("upgrade_add_cannon_base_value", 1);
	int m_base_upgrade_projectile_amount  = CFG_VALUE<int>("upgrade_projectile_amount_base_value", 1);


	// multipliers for the different rarities
	float m_multiplier_common	 = CFG_VALUE<float>("upgrade_common_multiplier", 1.0f);
	float m_multiplier_uncommon	 = CFG_VALUE<float>("upgrade_uncommon_multiplier", 1.2f);
	float m_multiplier_rare		 = CFG_VALUE<float>("upgrade_rare_multiplier", 1.4f);
	float m_multiplier_epic		 = CFG_VALUE<float>("upgrade_epic_multiplier", 1.7f);
	float m_multiplier_legendary = CFG_VALUE<float>("upgrade_legendary_multiplier", 2.0f);
	float m_multiplier_mythic	 = CFG_VALUE<float>("upgrade_mythical_multiplier", 3.0f);
};
