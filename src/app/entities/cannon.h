#pragma once
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <entities/director.h>

class FortunaDirector;

class Cannon : public PSInterfaces::IRenderable
{
public:
	Cannon();
	void update(const float dt) override;
	void render() override;

	Vector2 calculate_projectile_target_position();

	void fire();

	Vector2 position();
	void set_position(const Vector2& position);

	float rotation();
	void set_rotation(const float rotation);

	float range();
	void set_range(const float range);

	float fire_rate();
	void set_fire_rate(const float fire_rate);

	Vector2 projectile_target_position();
	void set_projectile_target_position(const Vector2& target_position);

	Texture2D texture();
	void set_texture(const Texture2D& texture);

	FortunaDirector* director();
	void set_director(FortunaDirector* director);

private:

	Vector2 m_c_position;
	float m_c_rotation;
	float m_c_range;
	float m_c_fire_rate;

	Texture2D m_c_texture;
	Rectangle m_c_source;
	Rectangle m_c_dest;

	Vector2 m_c_projectile_target_position;

	FortunaDirector* m_c_director;

};
