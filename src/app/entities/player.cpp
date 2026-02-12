#include "player.h"
#include <entities/director.h>
#include <raylib.h>

#include <pscore/application.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raymath.h>

#include <layers/applayer.h>
#include <layers/deathscreenlayer.h>
#include <layers/scorelayer.h>
#include <layers/uilayer.h>
#include <misc/smear.h>
#include <psinterfaces/entity.h>

#include <coordinatesystem.h>
#include <memory>

#ifndef CALCULATION_VELOCITY_MIN
#define CALCULATION_VELOCITY_MIN 2
#endif

#ifndef SCHMITT_TRIGGER_DELTA_ROTATION_MIN
#define SCHMITT_TRIGGER_DELTA_ROTATION_MIN 0.4
#endif

#ifndef SCHMITT_TRIGGER_DELTA_ROTATION_MAX
#define SCHMITT_TRIGGER_DELTA_ROTATION_MAX 0.5
#endif

Player::Player() : PSInterfaces::IEntity("player")
{
	Vector2 frame_grid{3, 4};
	m_sprite = PRELOAD_TEXTURE(ident_, "resources/entity/player.png", frame_grid);

	m_animation_controller = PSCore::sprites::SpriteSheetAnimation(
			FETCH_SPRITE_TEXTURE(ident_), {{1, 1, PSCore::sprites::KeyFrame, 1},
										   {3, 1, PSCore::sprites::KeyFrame, 3},
										   {3, 1, PSCore::sprites::KeyFrame, 3},
										   {3, 1, PSCore::sprites::KeyFrame, 3},
										   {10, 0.1, PSCore::sprites::Forward, 1}}
	);

	m_animation_controller.add_animation_at_index(0, 1);
	m_animation_controller.add_animation_at_index(2, 3);

	propose_z_index(0);

	// WARNING: THIS IS ONLY FOR TESTING
	if ( auto& vp = gApp()->viewport() ) {
		m_position = vp->viewport_base_size() / 2;
	}
	m_max_velocity = 200;
	m_rotation	   = 0;

	auto director	   = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	auto& direcor_vals = director->value_container_ref();
	set_interpolation_values(6, 2, 4, direcor_vals->player_input_velocity_mult, direcor_vals->player_input_rotation_mult, 30);
	set_texture_values(FETCH_SPRITE_TEXTURE(ident_), 90);
	//
	//
	set_max_velocity(direcor_vals->player_max_velocity);

	m_sails = std::make_shared<Sails>(this);

	m_sails->propose_z_index(30);
	m_sails->set_is_active(true);

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		app_layer->renderer()->submit_renderable(m_sails);
	}

	// Hit Flash Shader
	SetShaderValue(m_flash_shader, GetShaderLocation(m_flash_shader, "flash_color"), &m_flash_color, SHADER_UNIFORM_VEC4);
	m_flash_alpha_location = GetShaderLocation(m_flash_shader, "flash_alpha");

	// Upgrades
	std::vector<int> chances = {50, 25, 25};
	m_loot_table.add_loot_table(0, chances);
	chances = {30, 10, 40, 20};
	m_loot_table.add_loot_table(1, chances);
	chances = {99, 1};
	m_loot_table.add_loot_table(2, chances);

	m_loot_table.loot_table_values(1);
}

Player::~Player()
{
	UnloadShader(m_flash_shader);
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
		if ( IsKeyDown(KEY_D) &&
			 Vector2Length(m_velocity) - (m_velocity_rotation_downscale * fabsf(m_rotation_velocity)) > CALCULATION_VELOCITY_MIN ) {
			m_target_rotation += m_input_rotation_multiplier * dt;
		}
		if ( IsKeyDown(KEY_A) &&
			 Vector2Length(m_velocity) - (m_velocity_rotation_downscale * fabsf(m_rotation_velocity)) > CALCULATION_VELOCITY_MIN ) {
			m_target_rotation -= m_input_rotation_multiplier * dt;
		}
	}

	if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
		if ( director->player_health() > 0 ) {
			fire_cannons(dt);

			calculate_movement(dt);

			m_rotation_velocity = calculate_rotation_velocity(0.01, dt);

			reset_iframe(dt);

			// Animation Calculation

			if ( m_animation_controller.get_sprite_sheet_animation_index(3).value_or(2) == 2 &&
				 fabsf(m_rotation_velocity) > SCHMITT_TRIGGER_DELTA_ROTATION_MAX ) {
				m_sprite_sheet_animation_index = m_rotation_velocity < 0 ? 1 : 3;
				 }
			if ( m_animation_controller.get_sprite_sheet_animation_index(3).value_or(2) != 2 &&
				 fabsf(m_rotation_velocity) < SCHMITT_TRIGGER_DELTA_ROTATION_MIN ) {
				m_sprite_sheet_animation_index = 2;
				 }

			m_sprite_sheet_frame_index = static_cast<int>(round((Vector2Length(m_velocity) / m_max_velocity) * 2));

			m_animation_controller.set_animation_at_index(m_sprite_sheet_animation_index, m_sprite_sheet_frame_index, 3);
		}
		else {
			if ( m_animation_controller.get_sprite_sheet_frame_index(1) == 9 ) {
				set_is_active(false);
			}
		}
	}

	m_animation_controller.update_animation(dt);

	// Smear Calculation

	m_smear.update_smear(m_rotation - m_target_rotation, 0.5, 10, dt);

	// m_smear.update_smear(m_rotation_velocity, -10, 10, dt);

	if ( auto& vp = gApp()->viewport() ) {
		Vector2 position_absolute	 = vp->position_viewport_to_global(m_position);
		float scale					 = vp->viewport_scale();
		Vector2 smear_right_position = coordinatesystem::point_relative_to_global_leftup(position_absolute, m_rotation, Vector2Scale({18, 5}, scale));
		Vector2 smear_left_position =
				coordinatesystem::point_relative_to_global_leftdown(position_absolute, m_rotation, Vector2Scale({18, 5}, scale));
		Vector2 smear_forward_position =
				coordinatesystem::point_relative_to_global_rightup(position_absolute, m_rotation, Vector2Scale({20, 0}, scale));

		m_smear.calculate_linear_smear(smear_right_position, Vector2Length(m_velocity), m_rotation, 0.15f * scale, 0, 0);
		m_smear.calculate_linear_smear(smear_left_position, Vector2Length(m_velocity), m_rotation, 0.15f * scale, 0, 1);
		m_smear.calculate_exponential_smear(
				smear_forward_position, Vector2Length(m_velocity), m_rotation, 0.15f * scale, 0, 0.03f * scale, 0.05f * scale, 2
		);
		m_smear.calculate_exponential_smear(
				smear_forward_position, Vector2Length(m_velocity), m_rotation, 0.15f * scale, 0, -0.03f * scale, -0.05f * scale, 3
		);

		m_smear.add_smear_wave(0.1, 0.25, Vector2Length(m_velocity), m_max_velocity, dt, 0);

		m_smear.update_smear_wave({0, 1}, Linear, 1, 10, Vector2Length(m_velocity), m_max_velocity, dt);
	}
}

void Player::on_hit()
{
	if ( m_can_be_hit && !m_is_invincible ) {
		m_can_be_hit = false;
		if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
			director->set_player_health(director->player_health() - 1);
			if ( director->player_health() <= 0 ) {
				on_death();
			}
		}
	}
}

void Player::on_death()
{
	if (gApp()->get_layer<ScoreLayer>()) {
		gApp()->pop_layer<ScoreLayer>();
	}
	if (gApp()->get_layer<DeathScreenLayer>()) {
		gApp()->pop_layer<DeathScreenLayer>();
	}
	if (gApp()->get_layer<UILayer>()){
		gApp()->pop_layer<UILayer>();
	}

	gApp()->push_layer<ScoreLayer>();
	auto score_layer = gApp()->get_layer<ScoreLayer>();
	if (score_layer) {
		score_layer->set_layer_is_visible(false);
		score_layer->reset_state();
		score_layer->load_highscore(score_layer->score_filename());
		score_layer->save_new_highscore(
			dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
		score_layer->player_name_input = gApp()->current_player_name();
	}
	
	m_sails->set_is_active(false);
	for ( const auto& cannon: m_cannon_container ) {
		cannon->set_is_active(false);
	}
	m_animation_controller.set_animation_at_index(4, 0, 1);

	gApp()->get_layer<AppLayer>()->set_can_open_pause_menu(false);
	gApp()->push_layer<DeathScreenLayer>();
	auto death_layer = gApp()->get_layer<DeathScreenLayer>();
	if (death_layer && score_layer) {
		death_layer->set_score_layer_instance(score_layer);
		death_layer->set_score_should_be_saved(
			score_layer->check_for_new_highscore(
				dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty()
			)
		);
	}
}

void Player::set_is_invincible(bool invincible)
{
	m_is_invincible = invincible;
}

void Player::fire_cannons(float dt)
{
	switch ( m_fire_mode ) {

		case FireMode::InSequence: {
			m_time_since_last_shot_left += dt;
			m_time_since_last_shot_right += dt;
			if ( IsKeyDown(KEY_SPACE) && !m_fire_sequence_ongoing ) {
				m_fire_sequence_ongoing		  = true;
				m_fire_sequence_ongoing_right = true;
				m_fire_sequence_ongoing_left  = true;
			}

			if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !m_fire_sequence_ongoing && !m_fire_sequence_ongoing_right ) {
				m_fire_sequence_ongoing_right = true;
			}
			if ( m_time_since_last_shot_right > m_cannon_container.at(0)->fire_rate() / m_cannon_container.size() && m_fire_sequence_ongoing_right ) {
				m_cannon_container.at(m_firing_cannon_index.right)->fire();

				m_firing_cannon_index.right += 2;
				m_time_since_last_shot_right = 0;

				if ( m_firing_cannon_index.right >= m_cannon_container.size() ) {
					m_firing_cannon_index.right	  = 1;
					m_fire_sequence_ongoing_right = false;
					m_fire_sequence_ongoing		  = false;
				}
			}
			if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !m_fire_sequence_ongoing && !m_fire_sequence_ongoing_left ) {
				m_fire_sequence_ongoing_left = true;
			}

			if ( m_time_since_last_shot_left > m_cannon_container.at(0)->fire_rate() / m_cannon_container.size() && m_fire_sequence_ongoing_left ) {
				m_cannon_container.at(m_firing_cannon_index.left)->fire();
				m_firing_cannon_index.left += 2;
				m_time_since_last_shot_left = 0;
				if ( m_firing_cannon_index.left >= m_cannon_container.size() ) {
					m_firing_cannon_index.left	 = 0;
					m_fire_sequence_ongoing_left = false;
					m_fire_sequence_ongoing		 = false;
				}
			}
			break;
		}

		case FireMode::SameTime: {
			if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) ) {
				for ( auto cannon: m_cannon_container ) {
					if ( cannon->positioning() == Cannon::CannonPositioning::Left ) {
						cannon->fire();
					}
				}
			}
			if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ) {
				for ( auto cannon: m_cannon_container ) {
					if ( cannon->positioning() == Cannon::CannonPositioning::Right ) {
						cannon->fire();
					}
				}
			}
			if ( IsKeyDown(KEY_SPACE) ) {
				for ( auto cannon: m_cannon_container ) {
					cannon->fire();
				}
			}
			break;
		}
	}
}

void Player::render()
{
	// Draw Smear

	if ( auto& vp = gApp()->viewport() ) {
		m_smear.draw_smear(0, Linear, 2 * vp->viewport_scale(), 1, m_smear_color);
		m_smear.draw_smear(1, Linear, 2 * vp->viewport_scale(), 1, m_smear_color);
		m_smear.draw_smear(2, Exponential, 2 * vp->viewport_scale(), 1, m_smear_color);
		m_smear.draw_smear(3, Exponential, 2 * vp->viewport_scale(), 1, m_smear_color);
		m_smear.draw_smear_wave(Vector2Length(m_velocity), m_max_velocity, 2 * vp->viewport_scale(), 1, m_smear_color);
	}

	// Draw Ship

	BeginShaderMode(m_flash_shader);

	if ( auto& vp = gApp()->viewport() ) {
		vp->draw_in_viewport(
				m_texture, m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), m_position, m_rotation + m_rotation_offset, WHITE
		);
	}

	EndShaderMode();
}

void Player::reset_iframe(float dt)
{
	if ( !m_can_be_hit ) {
		if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
			m_iframe_timer += dt;
			if ( m_iframe_timer >= director->player_iframe_duration() ) {
				m_can_be_hit   = true;
				m_iframe_timer = 0;
			}
			float n_flash_lerp_scale = m_flash_lerp_scale / director->player_iframe_duration();
			m_flash_alpha			 = 0.5f * cosf(m_iframe_timer * sqrtf(m_iframe_timer) * n_flash_lerp_scale * sqrtf(n_flash_lerp_scale)) + 0.5;
		}
	}
	else {
		m_flash_alpha = 0;
	}

	SetShaderValue(m_flash_shader, m_flash_alpha_location, &m_flash_alpha, SHADER_UNIFORM_FLOAT);
}

void Player::draw_debug()
{
	if ( bounds().has_value() ) {
		for ( int i = 0; i < bounds().value().size(); i++ ) {
			if ( i < bounds().value().size() - 1 ) {
				DrawLineV(bounds().value().at(i), bounds().value().at(i + 1), GREEN);
			} else {
				DrawLineV(bounds().value().at(i), bounds().value().at(0), GREEN);
			}
		}
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
		const float input_rotation_multiplier, const float velocity_rotation_downscale
)
{
	m_acceleration_fade			  = acceleration_fade;
	m_deceleration_fade			  = deceleration_fade;
	m_rotation_fade				  = rotation_fade;
	m_input_velocity_multiplier	  = input_velocity_multiplier;
	m_input_rotation_multiplier	  = input_rotation_multiplier;
	m_velocity_rotation_downscale = velocity_rotation_downscale;
}

void Player::apply_repel_force(Vector2 repel_force)
{
	m_repel_velocity = Vector2Add(m_repel_velocity, repel_force);
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
					? Vector2Length(m_velocity) +
							  (m_target_velocity - Vector2Length(m_velocity) - (m_velocity_rotation_downscale * fabsf(m_rotation_velocity))) *
									  std::clamp(m_acceleration_fade * dt, 0.0f, 1.0f)
					: Vector2Length(m_velocity) +
							  (m_target_velocity - Vector2Length(m_velocity) - (m_velocity_rotation_downscale * fabsf(m_rotation_velocity))) *
									  std::clamp(m_deceleration_fade * dt, 0.0f, 1.0f);

	// Calculate with the Velocity Value and the Rotation the actual 2 Dimensional Velocity
	m_velocity.x = velocity_value * cos(m_rotation * DEG2RAD);
	m_velocity.y = velocity_value * sin(m_rotation * DEG2RAD);

	// Update Position based on Velocity
	m_position.x += (m_velocity.x + m_repel_velocity.x) * dt;
	m_position.y += (m_velocity.y + m_repel_velocity.y) * dt;

	m_repel_velocity = Vector2Subtract(m_repel_velocity, Vector2Scale(m_repel_velocity, 2.0f * dt));
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
		cannon_width = static_cast<float>(m_cannon_container[0]->texture().width / 7);
	}
	x_offset = -(((cannon_width / 4)) * m_cannon_container.size()) / 2;

	for ( int i = 0; i < 2; i++ ) {
		auto new_cannon = director->spawn_cannon(m_position);
		m_cannon_container.push_back(new_cannon);
		new_cannon->set_parent(m_shared_ptr_this);
		new_cannon->set_parent_position_x_offset(x_offset);
		new_cannon->set_parent_position_y_offset(new_cannon->texture().height / 3);
		new_cannon->set_shared_ptr_this(new_cannon);
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
	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(m_position);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points = {{20 * scale, 0 * scale},	{8 * scale, 6 * scale},	   {-15 * scale, 6 * scale},
												  {-20 * scale, 0 * scale}, {-15 * scale, -6 * scale}, {8 * scale, -6 * scale}};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, m_rotation, hitbox_points);
		}

	return std::nullopt;
}
void Player::set_input_velocity_multiplier(float val)
{
	m_input_velocity_multiplier = val;
};
void Player::set_input_rotation_multiplier(float val)
{
	m_input_rotation_multiplier = val;
};

Player::FireMode Player::fire_mode() const
{
	return m_fire_mode;
}

void Player::set_fire_mode(FireMode mode)
{
	m_fire_mode = mode;
}

//
// Player Sails
//
Sails::Sails(Player* player) : PSInterfaces::IEntity("player_sails"), m_player(player)
{
}

Sails::~Sails()
{
}

void Sails::update(float dt)
{
}

void Sails::render()
{
	if ( auto& vp = gApp()->viewport() ) {
		auto texture = m_player->m_sprite;
		vp->draw_in_viewport(
				texture->m_s_texture, m_player->m_animation_controller.get_source_rectangle(3).value_or(Rectangle{0}), m_player->m_position,
				m_player->m_rotation + m_player->m_rotation_offset, WHITE
		);
	}
}

void Sails::draw_debug()
{
}
