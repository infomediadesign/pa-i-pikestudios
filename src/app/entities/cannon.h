#pragma once
#include <memory>
#include <optional>
#include <pscore/sprite.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>

class FortunaDirector;
class Player;

class Cannon : public PSInterfaces::IRenderable
{
public:
	Cannon();
	void update(const float dt) override;
	void render() override;
	bool is_active() const override;

	void set_is_active(const bool active);

	enum class CannonPositioning { Left, Right };

	Vector2 calculate_projectile_target_position(); // Calculates the target position based on current position, rotation and range

	void fire(); // Spawns a projectile if the fire rate allows it

	void set_position_to_parent(); // Sets the cannon's position relative to its parent player

	void set_rotation_to_parent(); // Sets the cannon's rotation to match its parent player

	std::shared_ptr<Player> parent(); // Returns the shared pointer to the parent
	void set_parent(std::shared_ptr<Player> parent); // Sets the shared pointer to the parent

	std::optional<Vector2> position() const override; // Returns the cannon's position
	void set_position(const Vector2& position); // Sets the cannon's position

	float rotation(); // Returns the cannon's rotation
	void set_rotation(const float rotation); // Sets the cannon's rotation

	float range(); // Returns the cannon's range
	void set_range(const float range); // Sets the cannon's range

	float fire_rate(); // Returns the cannon's fire rate in seconds
	void set_fire_rate(const float fire_rate); // Sets the cannon's fire rate in seconds

	float projectile_speed(); // Returns the cannon's projectile speed
	void set_projectile_speed(const float projectile_speed); // Sets the cannon's projectile speed

	float dest_width(); // Returns the width of the destination rectangle
	float dest_height(); // Returns the height of the destination rectangle

	Vector2 projectile_target_position(); // Returns the projectile's target position
	void set_projectile_target_position(const Vector2& target_position); // Sets the projectile's target position

	float parent_position_x_offset(); // Returns the parent's position x offset
	void set_parent_position_x_offset(const float offset); // Sets the parent's position x offset

	float parent_position_y_offset(); // Returns the parent's position y offset
	void set_parent_position_y_offset(const float offset); // Sets the parent's position y offset

	CannonPositioning positioning(); // Returns the cannon's positioning
	void set_positioning(const Cannon::CannonPositioning positioning); // Sets the cannon's positioning

	Texture2D texture(); // Returns the cannon's texture
	void set_texture(const Texture2D& texture); // Sets the cannon's texture

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
	std::shared_ptr<PSCore::sprites::Sprite> m_c_sprite;
	bool m_c_is_active = true;

	std::shared_ptr<Player> m_c_parent;
};
