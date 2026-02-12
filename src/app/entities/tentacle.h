//
// Created by tylor on 30/01/2026.
//

#pragma once

#include <memory>


#include "pscore/collision.h"
#include "pscore/settings.h"
#include "pscore/sprite.h"
#include "psinterfaces/renderable.h"

class tentacle : public PSInterfaces::IRenderable
{
public:
	tentacle();
	~tentacle();

	void update(float dt) override;
	void render() override;
	void draw_debug() override;

	void set_pos(const Vector2& pos);

	void init(std::shared_ptr<tentacle> self, const Vector2& pos);

	std::optional<Vector2> position() const override;

	std::optional<std::vector<Vector2>> bounds() const override;

	void on_hit() override;
	
	void set_is_active(bool acive) override;

	enum State { Idle = 0, WaterBreak, Attacking, Retreat };

private:
	std::shared_ptr<tentacle> m_self;

	Vector2 m_pos{(float) 100, (float) 100};
	State m_state = State::Idle;
	State m_prev_state = m_state;
	std::string m_state_string;

	std::shared_ptr<PSCore::sprites::Sprite> m_Tentacle_sprite;
	std::unique_ptr<PSCore::collision::EntityCollider> m_collider;

	PSCore::sprites::SpriteSheetAnimation m_animation_controller;

	float time_until_water_break;
	float time_until_attack;
	float time_until_retreat;
	float until_reposition;

	float max_time_until_attack=0.8;
	float max_time_until_retreat=0.5;
	float max_until_reposition=0.6;
	
	int m_spawn_area_margin = CFG_VALUE<int>("tentacle_spawn_area_margin", 80);

	void IdleUpdate(float dt);
	void WaterBreakUpdate(float dt);
	void AttackingUpdate(float dt);
	void RetreatingUpdate(float dt);

	void SetNewPos();
};
