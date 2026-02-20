#include "lootchest.h"

LootChest::LootChest() : PSInterfaces::IEntity("loot_chest")
{
	IRenderable::propose_z_index(1);
	Vector2 frame_grid{1, 1};
	m_sprite = PRELOAD_TEXTURE(ident_, "resources/entity/test_loot.png", frame_grid);
	m_texture = m_sprite->m_s_texture;
}
