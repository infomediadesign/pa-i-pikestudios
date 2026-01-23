#pragma once
#include <memory>
#include <optional>
#include <pscore/sprite.h>
#include <pscore/collision.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>

class Player;
class FortunaDirector;

class Projectile : public PSInterfaces::IRenderable
{
public:
	Projectile();
	void update(const float dt) override;
	void render() override;

	std::optional<std::vector<Vector2>> bounds() const override;
	
	void init(const Vector2& position, std::shared_ptr<Projectile> self);

	void calculate_movement(
			const float dt, Vector2& target_position
	); // Calculates the movement towards the target position wihle keeping the forward velocity of the owner

	Texture2D texture(); // Returns the texture of the projectile
	void set_texture(const Texture2D& texture); // Sets the texture of the projectile

	std::optional<Vector2> position() const override; // Returns the position of the projectile
	void set_position(const Vector2& position); // Sets the position of the projectile

	float rotation(); // Returns the rotation of the projectile
	void set_rotation(const float& rotation); // Sets the rotation of the projectile

	Vector2 velocity(); // Returns the velocity of the projectile
	void set_velocity(const Vector2& velocity); // Sets the velocity of the projectile

	Vector2 target_position(); // Returns the target position of the projectile
	void set_target_position(const Vector2& target_position); // Sets the target position of the projectile

	Vector2 direction(); // Returns the direction of the projectile
	void set_direction(const Vector2& direction); // Sets the direction of the projectile

	Vector2 movement(); // Returns the movement of the projectile
	void set_movement(const Vector2& movement); // Sets the movement of the projectile

	float speed(); // Returns the speed of the projectile
	void set_speed(const float speed); // Sets the speed of the projectile

	float travel_distance(); // Returns the travel distance of the projectile
	void set_travel_distance(const float travel_distance); // Sets the travel distance of the projectile

	std ::shared_ptr<Projectile> shared_ptr(); // Returns the shared pointer to this projectile
	void set_shared_ptr(std::shared_ptr<Projectile>& ptr); // Sets the shared pointer to this projectile

	std::shared_ptr<Player> owner(); // Returns shared pointer to the owner of the projectile
	void set_owner(std::shared_ptr<Player>& owner); // Sets shared pointer to the owner of the projectile

	Vector2 owner_velocity(); // Returns the owner's velocity
	void set_owner_velocity(const Vector2& velocity); // Sets the owner's velocity

private:
	Vector2 m_p_position;
	Vector2 m_p_velocity;
	Vector2 m_p_target_position;
	Vector2 m_p_direction;
	Vector2 m_p_movement;
	float m_p_rotation;
	float m_p_speed;
	float m_p_travel_distance;
	Vector2 m_p_owner_velocity;

	Texture2D m_p_texture;
	std::shared_ptr<PSCore::sprites::Sprite> m_p_sprite;
	Rectangle m_p_source;
	Rectangle m_p_dest;
	bool m_p_is_first_tick = true;
	std::shared_ptr<Projectile> m_p_shared_ptr;
	std::shared_ptr<Player> m_p_owner;
	
	std::unique_ptr<PSCore::collision::EntityCollider> m_collider;
};
