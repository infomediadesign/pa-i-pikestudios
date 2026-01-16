#pragma once
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <entities/director.h>
#include <entities/player.h>

class FortunaDirector;
class Player;

class Cannon : public PSInterfaces::IRenderable
{
public:
	Cannon();
	void update(const float dt) override;
	void render() override;

	enum class CannonPositioning { Left, Right };

	Vector2 calculate_projectile_target_position();
	void fire();
	void set_position_to_parent();
	void set_rotation_to_parent();

	std::shared_ptr<Player> parent();
	void set_parent(std::shared_ptr<Player> parent);

	Vector2 position();
	void set_position(const Vector2& position);

	float rotation();
	void set_rotation(const float rotation);

	float range();
	void set_range(const float range);

	float fire_rate();
	void set_fire_rate(const float fire_rate);

	float projectile_speed();
	void set_projectile_speed(const float projectile_speed);

	Vector2 projectile_target_position();
	void set_projectile_target_position(const Vector2& target_position);

	float parent_position_x_offset();
	void set_parent_position_x_offset(const float offset);

	float parent_position_y_offset();
	void set_parent_position_y_offset(const float offset);

	CannonPositioning positioning();
	void set_positioning(const Cannon::CannonPositioning positioning);

	Texture2D texture();
	void set_texture(const Texture2D& texture);

	FortunaDirector* director();
	void set_director(FortunaDirector* director);

private:

	Vector2 m_c_position;
	float m_c_rotation;
	float m_c_range;
	float m_c_projectile_speed;
	float m_c_fire_rate_in_s;
	float m_c_time_since_last_shot;
	Vector2 m_c_projectile_target_position;
	float m_c_parent_position_x_offset;
	float m_c_parent_position_y_offset;
	CannonPositioning m_c_positioning;

	Texture2D m_c_texture;
	Rectangle m_c_source;
	Rectangle m_c_dest;

	FortunaDirector* m_c_director;
	std::shared_ptr<Player> m_c_parent;
};
