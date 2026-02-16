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

	void test_upgrade();

	void draw_upgrade_cards();

public:

	std::vector<int> m_chances{50, 30, 20,};
	Texture2D m_card_texture;
	LootTable m_loot_table;
};
