#pragma once

#include <memory>
#include <misc/smear.h>
#include <optional>
#include <pscore/collision.h>
#include <pscore/settings.h>
#include <pscore/sprite.h>
#include <psinterfaces/entity.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <string>

class FortunaDirector;

class Fin;
class Body;
class Shark : public PSInterfaces::IRenderable
{
	friend class Body;
	friend class Fin;

public:
	Shark();
	~Shark();

	void update(float dt) override;
	void render() override;
	void draw_debug() override;
	void on_hit() override;
	void set_is_active(bool active) override;

	void set_pos(const Vector2& pos);

	void init(std::shared_ptr<Shark> self, const Vector2& pos);

	std::optional<Vector2> position() const override;

	std::optional<std::vector<Vector2>> bounds() const override;

	enum State { Idle = 0, Pursuing, Attacking, Retreat };

	float calculate_rotation_velocity(float frequency, float dt);

protected:
	std::shared_ptr<PSCore::sprites::Sprite> m_shark_sprite;

	void determined_if_marked();


	bool m_marked;

	std::shared_ptr<Shark> m_self;
	std::shared_ptr<Body> m_body;
	std::shared_ptr<Fin> m_fin;
	FortunaDirector* m_director;

	Vector2 m_pos{(float) 100, (float) 100};
	float m_speed					  = CFG_VALUE<float>("shark_speed", 100.0f);
	float m_pursue_stop_distance	  = CFG_VALUE<float>("shark_pursue_stop_distance", 20.0f);
	float m_retreat_reengage_distance = CFG_VALUE<float>("shark_retreat_reengage_distance", 40.0f);
	float m_retreat_speed			  = CFG_VALUE<float>("shark_retreat_speed", 20.0f);
	float m_drop_upgrade_chance		  = CFG_VALUE<float>("shark_drop_upgrade_chance", 10.0f);

	State m_state = State::Idle;
	std::string m_state_string;

	// Horde behavior
	float m_horde_separation_distance = CFG_VALUE<float>("shark_separation_distance", 30.0f);
	float m_horde_separation_strength = CFG_VALUE<float>("shark_separation_strength", 120.0f);
	float m_horde_cohesion_radius	  = CFG_VALUE<float>("shark_cohesion_radius", 200.0f);
	float m_horde_cohesion_strength	  = CFG_VALUE<float>("shark_cohesion_strength", 50.0f);
	bool m_horde_sync_rotation		  = CFG_VALUE<bool>("shark_sync_rotation", false);

	std::unique_ptr<PSCore::collision::EntityCollider> m_collider;

	float m_shark_rotation	  = 0;
	float m_rotation_velocity = 0;

	PSCore::sprites::SpriteSheetAnimation m_animation_controller;
	Vector2 m_smear_origin{5, -0.5};
};

class Fin : public PSInterfaces::IRenderable
{
	friend class Shark;

public:
	Fin(Shark* shark);
	~Fin();

	void render() override;
	void update(float dt) override;
	void draw_debug() override;

private:
	const Shark* m_shark;
	Vector2 m_size{20.0f, 60.0f};

	Smear m_smear;
	Color m_smear_color = {9, 75, 101, 127};
};

class Body : public PSInterfaces::IRenderable
{
	friend class Shark;

public:
	Body(Shark* shark);
	~Body();

	void render() override;
	void update(float dt) override;
	void draw_debug() override;

private:
	const Shark* m_shark;
	Vector2 m_size{60.0f, 120.0f};
	Vector2 m_texture_size;
	Vector4 m_shader_color{251, 206, 39, 255};
	Shader m_outline_shader = LoadShader(0, "resources/shader/2d_outline.fs");
};
