#include "layers/upgradelayer.h"
#include <iostream>
#include "pscore/sprite.h"
#include "pscore/viewport.h"
#include <raylib.h>
#include "raygui.h"

UpgradeLayer::UpgradeLayer()
{
	Vector2 frame_grid{1, 1};
	m_card_texture = PRELOAD_TEXTURE("card", "resources/ui/test_upgrade_card.png", frame_grid)->m_s_texture;
	
	m_loot_table.add_loot_table(0, m_chances);
}

void UpgradeLayer::on_update(float dt)
{
}

void UpgradeLayer::on_render()
{
	draw_upgrade_cards();
}

void UpgradeLayer::test_upgrade()
{
	printf("Upgrade Clicked\n");
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

	if ( GuiButtonTexture(m_card_texture, screen_middel, 0, scale, WHITE, GRAY, "") ) {
		test_upgrade();
	}

	if ( GuiButtonTexture(m_card_texture, {screen_middel.x + m_card_texture.width + 16, screen_middel.y}, 0, scale, WHITE, GRAY, "") ) {
		test_upgrade();
	}

	if ( GuiButtonTexture(m_card_texture, {screen_middel.x - m_card_texture.width - 16, screen_middel.y}, 0, scale, WHITE, GRAY, "") ) {
		test_upgrade();
	}

}