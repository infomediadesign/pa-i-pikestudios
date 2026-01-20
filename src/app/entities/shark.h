#pragma once

#include <memory>
#include <psinterfaces/entity.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <string>
#include "pscore/sprite.h"

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

	enum State { Idle = 0, Pursuing, Attacking, Retreat };

private:
	bool m_marked;
	
	std::shared_ptr<Body> m_body;
	std::shared_ptr<Fin> m_fin;

	Vector2 m_pos{(float) 100, (float) 100};
	float m_speed = 100.0f;
	State m_state = State::Idle;
	std::string m_state_string;

	std::shared_ptr<PSCore::sprites::Sprite> m_shark_sprite;

	float m_shark_rotation = 0;
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
	Vector2 m_size{20, 60};
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
	Vector2 m_size{60, 120};
};
