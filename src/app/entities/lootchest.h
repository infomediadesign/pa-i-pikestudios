#pragma once
#include <psinterfaces/renderable.h>
#include <pscore/sprite.h>

class LootChest : public PSInterfaces::IRenderable
{
	public:
	LootChest();
	void update(float dt) override;
	void render() override;

	private:
	Texture2D m_texture;
	std::shared_ptr<PSCore::sprites::Sprite> m_sprite;

};
