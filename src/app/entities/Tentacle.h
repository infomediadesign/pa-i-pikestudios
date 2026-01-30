//
// Created by tylor on 30/01/2026.
//

#ifndef FORTUNASECHO_TANTACLE_H
#define FORTUNASECHO_TANTACLE_H
#include <memory>


#include "pscore/collision.h"
#include "pscore/sprite.h"
#include "psinterfaces/renderable.h"


class Tentacle : public PSInterfaces::IRenderable
{
public:
	Tentacle();
	~Tentacle();

	void update(float dt) override;
	void render() override;
	void draw_debug() override;

	void set_pos(const Vector2& pos);

	void init(std::shared_ptr<Tentacle> self, const Vector2& pos);

	std::optional<Vector2> position() const override;

	std::optional<std::vector<Vector2>> bounds() const override;

	void on_hit() override;

	enum State { Idle = 0, Attacking, Retreat };

private:
	std::shared_ptr<Tentacle> m_self;

	Vector2 m_pos{(float) 100, (float) 100};
	State m_state = State::Idle;
	std::string m_state_string;

	std::shared_ptr<PSCore::sprites::Sprite> m_Tentacle_sprite;
	std::unique_ptr<PSCore::collision::EntityCollider> m_collider;

	PSCore::sprites::SpriteSheetAnimation m_animation_controller;

	float time_until_attack;
	float time_until_retreat;
	float until_reposition;

	float max_time_until_attack=1;
	float max_time_until_retreat=1;
	float max_until_reposition=1;

	void IdleUpdate(float dt);
	void AttackingUpdate(float dt);
	void RetreatingUpdate(float dt);

	void SetNewPos();









};


#endif // FORTUNASECHO_TANTACLE_H
