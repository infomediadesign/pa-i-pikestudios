#pragma once
#include <psinterfaces/renderable.h>
#include <raylib.h>

class Projectile : public PSInterfaces::IRenderable
{
public:
	Projectile();
	void update(const float dt) override;
	void render() override;
	void initialize(const Vector2& position, const float& rotation, const float& velocity, const Texture2D& texture);
	void calculate_movement(const float dt, Vector2& target_position);


	Texture2D texture();
	void set_texture(const Texture2D& texture);

	Vector2 position();
	void set_position(const Vector2& position);

	float rotation();
	void set_rotation(const float& rotation);

	Vector2 velocity();
	void set_velocity(const Vector2& velocity);

	Vector2 target_position();
	void set_target_position(const Vector2& target_position);

	Vector2 direction();
	void set_direction(const Vector2& direction);

	Vector2 movement();
	void set_movement(const Vector2& movement);

	float speed();
	void set_speed(const float speed);

	float travel_distance();
	void set_travel_distance(const float travel_distance);

private:
	Vector2 m_p_position;
	Vector2 m_p_velocity;
	Vector2 m_p_target_position;
	Vector2 m_p_direction;
	Vector2 m_p_movement;
	float m_p_rotation;
	float m_p_speed;
	float m_p_travel_distance;

	Texture2D m_p_texture;
	Rectangle m_p_source;
	Rectangle m_p_dest;
	bool m_p_is_first_tick = true;
};
