#include "player.h"
#include <entities/director.h>
#include <raylib.h>

#include <pscore/application.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raymath.h>

#include <layers/applayer.h>
#include <misc/smear.h>
#include <psinterfaces/entity.h>

#include <coordinatesystem.h>

#ifndef CALCULATION_VELOCITY_MIN
#define CALCULATION_VELOCITY_MIN 1
#endif

#ifndef CALCULATION_DELTA_ROTATION_MIN
#define CALCULATION_DELTA_ROTATION_MIN 0.5
#endif

Player::Player() : PSInterfaces::IEntity("player")
{
	Vector2 frame_grid{3, 4};
	m_sprite = PRELOAD_TEXTURE(ident_, "ressources/entity/Player.png", frame_grid);

	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(
			FETCH_SPRITE_TEXTURE(ident_), {{3, 1, PSCore::sprites::KeyFrame, 1},
										   {3, 1, PSCore::sprites::KeyFrame, 3},
										   {3, 1, PSCore::sprites::KeyFrame, 3},
										   {3, 1, PSCore::sprites::KeyFrame, 3}}
	);

	m_animation_controller.add_animation_at_index(0, 1);
	m_animation_controller.add_animation_at_index(2, 3);

	// WARNING: THIS IS ONLY FOR TESTING
	if ( auto& vp = gApp()->viewport() ) {
		m_position = vp->viewport_base_size() / 2;
	}
	m_max_velocity = 200;
	m_rotation	   = 0;
	set_interpolation_values(6, 2, 4, 1500, 0.9);
	set_texture_values(FETCH_SPRITE_TEXTURE(ident_), 90);
	//
	//
}

void Player::update(const float dt)
{
	if ( !m_is_clone ) {
		// Input Functions to set Target Velocity and Target Rotation
		if ( IsKeyDown(KEY_W) ) {
			m_target_velocity += m_target_velocity < m_max_velocity ? m_input_velocity_multiplier * dt : 0;
		}
		if ( IsKeyDown(KEY_S) ) {
			m_target_velocity -= m_target_velocity > 0 ? m_input_velocity_multiplier * dt : 0;
		}
		if ( IsKeyDown(KEY_D) && Vector2Length(m_velocity) > CALCULATION_VELOCITY_MIN ) {
			m_target_rotation += m_input_rotation_multiplier * Vector2Length(m_velocity) * dt;
		}
		if ( IsKeyDown(KEY_A) && Vector2Length(m_velocity) > CALCULATION_VELOCITY_MIN ) {
			m_target_rotation -= m_input_rotation_multiplier * Vector2Length(m_velocity) * dt;
		}
	}

	calculate_movement(dt);

	m_rotation_velocity = calculate_rotation_velocity(0.01, dt);

	// Animation Calculation

	int sprite_sheet_animation_index;
	if ( m_rotation_velocity > CALCULATION_DELTA_ROTATION_MIN || m_rotation_velocity < -CALCULATION_DELTA_ROTATION_MIN ) {
		sprite_sheet_animation_index = m_rotation_velocity < 0 ? 1 : 3;
	} else {
		sprite_sheet_animation_index = 2;
	}

	int sprite_sheet_frame_index = static_cast<int>(round((Vector2Length(m_velocity) / m_max_velocity) * 2));

	m_animation_controller.set_animation_at_index(sprite_sheet_animation_index, sprite_sheet_frame_index, 3);

	m_animation_controller.update_animation(dt);

	// Smear Calculation

	smear::update_smear_rotation(&m_smear_rotation, m_rotation - m_target_rotation, 0.5, 10, dt);

	if ( auto& vp = gApp()->viewport() ) {
		Vector2 m_position_absolute = vp->position_viewport_to_global(m_position);
		Vector2 m_smear_right_position =
				coordinatesystem::point_relative_to_global_leftup(m_position_absolute, m_rotation, Vector2Scale({18, 5}, vp->viewport_scale()));
		Vector2 m_smear_left_position =
				coordinatesystem::point_relative_to_global_leftdown(m_position_absolute, m_rotation, Vector2Scale({18, 5}, vp->viewport_scale()));

		m_smear_points[0] = smear::calculate_smear_linear_points(
				m_smear_right_position, Vector2Length(m_velocity), m_rotation, m_smear_rotation, 0.15 * vp->viewport_scale(), 0
		);
		m_smear_points[1] = smear::calculate_smear_linear_points(
				m_smear_left_position, Vector2Length(m_velocity), m_rotation, m_smear_rotation, 0.15 * vp->viewport_scale(), 0
		);

		m_smear_wave_time += dt;
		if ( m_smear_wave_time >= m_smear_wave_per_second / (Vector2Length(m_velocity) / m_max_velocity) ) {
			m_smear_wave_time = 0;
			smear::send_smear_wave(&m_smear_wave, &m_smear_wave_index, 0.1, Vector2Length(m_velocity), m_max_velocity, dt);
		}
		smear::update_smear_wave(&m_smear_wave, 1, Vector2Length(m_velocity), m_max_velocity, dt);
		smear::calculate_smear_wave_points(
				&m_smear_wave_points, m_smear_wave, m_smear_points, {0, 1}, m_smear_rotation, 10, Vector2Length(m_velocity), m_max_velocity,
				smear::linear_points
		);
	}
}

void Player::damage()
{
	PS_LOG(LOG_INFO, "player took damage");
}

void Player::render()
{
	// Draw Smear

	if ( auto& vp = gApp()->viewport() ) {
		smear::draw_smear_linear(m_smear_points[0], 2 * vp->viewport_scale(), 1, BLUE);
		smear::draw_smear_linear(m_smear_points[1], 2 * vp->viewport_scale(), 1, BLUE);
		smear::draw_smear_wave_between_smears(
				m_smear_wave_points, m_smear_wave, Vector2Length(m_velocity), m_max_velocity, 2 * vp->viewport_scale(), 1, SKYBLUE
		);
	}

	// Draw Ship

	if ( auto& vp = gApp()->viewport() ) {
		vp->draw_in_viewport(
				m_texture, m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_position, m_rotation + m_rotation_offset, WHITE
		);
		vp->draw_in_viewport(
				m_texture, m_animation_controller.get_source_rectangle(3).value_or(Rectangle{0}), m_position, m_rotation + m_rotation_offset, WHITE
		);
	}
}

std::optional<Vector2> Player::position() const
{
	return m_position;
}

Vector2 Player::velocity()
{
	return m_velocity;
}

float Player::target_velocity()
{
	return m_target_velocity;
}

float Player::max_velocity()
{
	return m_max_velocity;
}

float Player::rotation()
{
	return m_rotation;
}

float Player::target_rotation()
{
	return m_target_rotation;
}

void Player::set_position(const Vector2& position)
{
	m_position = position;
}

void Player::set_velocity(const Vector2& velocity)
{
	// Check if the input Velocity is in range of the Max Velocity
	// and set the Velocity and Target Velocity to the input Velocity if the check is True
	// or if the check is False it's set to Max Velocity
	m_velocity		  = Vector2Length(velocity) <= m_max_velocity ? velocity : Vector2Scale(Vector2Normalize(velocity), m_max_velocity);
	m_target_velocity = Vector2Length(velocity) <= m_max_velocity ? Vector2Length(velocity) : m_max_velocity;
}

void Player::set_target_velocity(const float target_velocity)
{
	m_target_velocity = target_velocity;
}

void Player::set_max_velocity(const float max_velocity)
{
	m_max_velocity = max_velocity;
}

void Player::set_rotation(const float rotation)
{
	m_rotation		  = rotation;
	m_target_rotation = rotation;
}

void Player::set_target_rotation(const float target_rotation)
{
	m_target_rotation = target_rotation;
}

void Player::set_interpolation_values(
		const float acceleration_fade, const float deceleration_fade, const float rotation_fade, const float input_velocity_multiplier,
		const float input_rotation_multiplier
)
{
	m_acceleration_fade			= acceleration_fade;
	m_deceleration_fade			= deceleration_fade;
	m_rotation_fade				= rotation_fade;
	m_input_velocity_multiplier = input_velocity_multiplier;
	m_input_rotation_multiplier = input_rotation_multiplier;
}

void Player::calculate_movement(const float dt)
{
	// Linear Interpolation form Rotation to Target Rotation with a regression of Rotation and a static Alpha
	// which ends in an exponential approximation to calculate the rotation
	m_rotation = m_rotation + (m_target_rotation - m_rotation) * std::clamp(m_rotation_fade * dt, 0.0f, 1.0f);

	// Check if the Velocity should increase or decrease and uses right the Linear Interpolation form Velocity to Target Velocity with a regression of
	// Velocity and a static Alpha which ends in an exponential approximation to calculate the Value of the Velocity
	float velocity_value =
			(m_target_velocity - Vector2Length(m_velocity)) > 0
					? Vector2Length(m_velocity) + (m_target_velocity - Vector2Length(m_velocity)) * std::clamp(m_acceleration_fade * dt, 0.0f, 1.0f)
					: Vector2Length(m_velocity) + (m_target_velocity - Vector2Length(m_velocity)) * std::clamp(m_deceleration_fade * dt, 0.0f, 1.0f);

	// Calculate with the Velocity Value and the Rotation the actual 2 Dimensional Velocity
	m_velocity.x = velocity_value * cos(m_rotation * DEG2RAD);
	m_velocity.y = velocity_value * sin(m_rotation * DEG2RAD);

	// Update Position based on Velocity
	m_position.x += m_velocity.x * dt;
	m_position.y += m_velocity.y * dt;
}

float Player::calculate_rotation_velocity(float frequency, float dt)
{
	static float timer			   = 0;
	static float last_rotation	   = 0;
	static float rotation_velocity = 0;

	timer += dt;

	if ( timer >= frequency ) {
		timer = 0;

		rotation_velocity = m_rotation - last_rotation;

		last_rotation = m_rotation;
	}

	return rotation_velocity;
}


void Player::set_texture_values(const Texture2D& texture, const float rotation_offset)
{
	m_texture		  = texture;
	m_rotation_offset = rotation_offset;
}

void Player::initialize_cannon()
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( !director ) {
		return;
	}

	float cannon_width = 5.0f;

	float x_offset = 0;
	if ( !m_cannon_container.empty() ) {
		cannon_width = static_cast<float>(m_cannon_container[0]->texture().width);
	}
	x_offset = -((cannon_width + cannon_width / 4) * m_cannon_container.size()) / 2;

	for ( int i = 0; i < 2; i++ ) {
		auto new_cannon = director->spawn_cannon(m_position);
		m_cannon_container.push_back(new_cannon);
		new_cannon->set_parent(m_shared_ptr_this);
		new_cannon->set_parent_position_x_offset(x_offset);
		new_cannon->set_parent_position_y_offset(new_cannon->texture().height);
		if ( i == 0 ) {
			new_cannon->set_positioning(Cannon::CannonPositioning::Left);
		} else {
			new_cannon->set_positioning(Cannon::CannonPositioning::Right);
		}
	}
}

void Player::add_cannons(int amount)
{
	for ( int i = 0; i < amount; i++ ) {
		initialize_cannon();
	}
}

// Border Collision Variables and Methods
void Player::set_border_collision_active_horizontal(bool active)
{
	m_border_collision_active_horizontal = active;
}

bool Player::border_collision_active_horizontal() const
{
	return m_border_collision_active_horizontal;
}

void Player::set_border_collision_active_vertical(bool active)
{
	m_border_collision_active_vertical = active;
}

bool Player::border_collision_active_vertical() const
{
	return m_border_collision_active_vertical;
}

void Player::set_is_clone(bool active)
{
	m_is_clone = active;
}

bool Player::is_clone() const
{
	return m_is_clone;
}

float Player::dest_width() const
{
	return m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}).width;
}

float Player::dest_height() const
{
	return m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}).height;
}

std::vector<std::shared_ptr<Cannon>>& Player::cannon_container()
{
	return m_cannon_container;
}

void Player::set_cannon_container(const std::vector<std::shared_ptr<Cannon>>& container)
{
	m_cannon_container = container;
}

std::shared_ptr<Player> Player::shared_ptr_this()
{
	return m_shared_ptr_this;
}

void Player::set_shared_ptr_this(std::shared_ptr<Player> ptr)
{
	m_shared_ptr_this = ptr;
}

std::optional<std::vector<Vector2>> Player::bounds() const
{
	Rectangle shark_rec;
	shark_rec = m_sprite->frame_rect({0, 0});

	std::vector<Vector2> v{
			m_position, // Top-left
			Vector2{m_position.x + shark_rec.width, m_position.y}, // Top-right
			Vector2{m_position.x + shark_rec.width, m_position.y + shark_rec.height}, // Bottom-right
			Vector2{m_position.x, m_position.y + shark_rec.height} // Bottom-left
	};

	return v;
}
void Player::set_input_velocity_multiplier(float val) {
	m_input_velocity_multiplier = val;
};
void Player::set_input_rotation_multiplier(float val) {
	m_input_rotation_multiplier = val;
};
