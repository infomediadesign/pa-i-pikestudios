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

	void apply_upgrade(int upgrdae_index);

	void draw_upgrade_cards();

	void print_loot_table_values(std::vector<LootTableValue>);


	std::vector<int> m_chances{33, 33, 34,};
	Texture2D m_card_texture;
	LootTable m_loot_table;
	std::vector<LootTableValue> m_current_loot_table_values;
};
