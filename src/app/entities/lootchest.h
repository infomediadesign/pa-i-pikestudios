#pragma once
#include <pscore/collision.h>
#include <pscore/sprite.h>
#include <psinterfaces/renderable.h>

class LootChest : public PSInterfaces::IRenderable
{
public:
	LootChest();
	void update(float dt) override;
	void render() override;
	void on_hit() override;
	std::optional<std::vector<Vector2>> bounds() const override;
	std::optional<Vector2> position() const override;

	void set_position(const Vector2& position);
	void set_rotation(const float& rotation);

	void init(const Vector2& position, std::shared_ptr<LootChest> self);

	void draw_debug() override;

private:
	Texture2D m_texture;
	Vector2 m_position;
	float m_rotation = 0;
	std::shared_ptr<PSCore::sprites::Sprite> m_sprite;

	std::unique_ptr<PSCore::collision::EntityCollider> m_collider;
	PSCore::sprites::SpriteSheetAnimation m_anim_controller;
};
