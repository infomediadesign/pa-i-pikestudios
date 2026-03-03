#pragma once

#include <memory>
#include <optional>
#include <psinterfaces/renderable.h>

#include <entities/cannon.h>
#include <raylib.h>

#include <misc/smear.h>

#include <misc/loottable.h>
#include <pscore/sprite.h>

class Sails;

class PlayerPriv;
class Player : public PSInterfaces::IRenderable
{
	friend class Sails;

public:
	Player();

	~Player();

	void update(float dt) override;

	void render() override;

	void draw_debug() override;

	void on_hit() override;

	void on_death();

	std::optional<Vector2> position() const override;

	std::optional<std::vector<Vector2>> bounds() const override;

	virtual std::optional<Vector2> velocity() const override;

	virtual std::optional<float> rotation() const override;

	float target_velocity();

	float max_velocity();

	float target_rotation();

	void set_position(const Vector2& position);

	void set_velocity(const Vector2& velocity);

	void set_target_velocity(float target_velocity);

	void set_max_velocity(float max_velocity);

	void set_input_velocity_multiplier(float val);
	void set_input_rotation_multiplier(float val);

	void set_rotation(float rotation);

	void set_target_rotation(float target_rotation);

	void set_interpolation_values(
			float acceleration_fade, float deceleration_fade, float rotation_fade, float input_velocity_multiplier, float input_rotation_multiplier,
			float velocity_rotation_downscale
	);

	void calculate_movement(float dt);

	float calculate_rotation_velocity(float frequency, float dt);

	void set_texture_values(const Texture2D& texture, float rotation_offset);

	// Borderinteration Variables and Methods
	void set_border_collision_active_horizontal(bool active);

	bool border_collision_active_horizontal() const;

	void set_border_collision_active_vertical(bool active);

	bool border_collision_active_vertical() const;

	void set_is_clone(bool active);

	bool is_clone() const;

	std::optional<Vector2> size() const override;

	// Cannons & Projectiles Variables and Methods
	std::vector<std::shared_ptr<Cannon>>& cannon_container();
	void set_cannon_container(const std::vector<std::shared_ptr<Cannon>>& container);

	std::shared_ptr<Player> shared_ptr_this();
	void set_shared_ptr_this(std::shared_ptr<Player> ptr);

	void initialize_cannon();

	void fire_cannons(float dt);

	enum class FireMode { InSequence, SameTime };

	FireMode fire_mode() const;
	void set_fire_mode(FireMode mode);

	struct FiringCannonIndex
	{
		int right;
		int left;
	};

	void add_cannons(int amount);

	// invincibleity
	void reset_iframe(float dt);
	void set_is_invincible(bool invincible);

	void apply_repel_force(Vector2 repel_force);

	void enable_explosive_barrels();
	void set_barrel_intervall(float intervall);

	// Sound
    void play_sound(Sound& sound, float volume, float pitch);

private:
	std::unique_ptr<PlayerPriv> _p;
};

class Sails : public PSInterfaces::IRenderable
{
public:
	Sails(Player* player);
	~Sails();

	void update(float dt) override;
	void render() override;
	void draw_debug() override;

private:
	const Player* m_player;
	std::shared_ptr<PSCore::sprites::Sprite> m_sprite;

	// Shader
	Shader m_emissive_shader = LoadShader(NULL, "resources/shader/emissive_color.fs");
	Vector3 m_emissive_color = {255, 255, 255};
	int m_emissive_texture_location;
};
