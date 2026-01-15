#pragma once

#include <memory>
#include <psinterfaces/entity.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>

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
	Shark* m_shark;
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
	Shark* m_shark;
	Vector2 m_size{60, 120};
};

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

private:
	std::shared_ptr<Body> m_body;
	std::shared_ptr<Fin> m_fin;

	Vector2 m_pos{(float) GetScreenWidth() / 2, (float) GetScreenHeight() / 2};
	
	Vector2 pos_to_rel_fin();
};
