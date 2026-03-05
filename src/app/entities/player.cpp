#include "player.h"
#include <cstdint>
#include <entities/director.h>
#include <raylib.h>

#include <pscore/application.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raymath.h>

#include <entities/explosivebarrel.h>
#include <layers/applayer.h>
#include <layers/deathscreenlayer.h>
#include <layers/scorelayer.h>
#include <layers/uilayer.h>
#include <misc/smear.h>
#include <pscore/spawner.h>
#include <psinterfaces/entity.h>

#include <coordinatesystem.h>
#include <memory>
#include "pscore/settings.h"

#ifndef CALCULATION_VELOCITY_MIN
#define CALCULATION_VELOCITY_MIN 2
#endif

#ifndef SCHMITT_TRIGGER_DELTA_ROTATION_MIN
#define SCHMITT_TRIGGER_DELTA_ROTATION_MIN 0.4
#endif

#ifndef SCHMITT_TRIGGER_DELTA_ROTATION_MAX
#define SCHMITT_TRIGGER_DELTA_ROTATION_MAX 0.5
#endif


class PlayerPriv
{
	friend class Player;
	friend class Sails;
	// Base Movement Variables
	Vector2 m_position		 = {0};
	Vector2 m_velocity		 = {0};
	Vector2 m_repel_velocity = {0, 0};
	float m_max_velocity	 = 200;
	float m_rotation		 = 0;

	// Interpolation Values for the Movement Calculation
	float m_target_velocity				= 0;
	float m_target_rotation				= 0;
	float m_acceleration_fade			= 0;
	float m_deceleration_fade			= 0;
	float m_rotation_fade				= 0;
	float m_input_velocity_multiplier	= 0;
	float m_input_rotation_multiplier	= 0;
	float m_rotation_velocity			= 0;
	float m_velocity_rotation_downscale = 0;

	// Variables for Texture Rendering
	float m_rotation_offset = 0;
	std::shared_ptr<PSCore::sprites::Sprite> m_sprite;

	// Variables for Animation
	PSCore::sprites::SpriteSheetAnimation m_animation_controller;
	int m_sprite_sheet_animation_index = 2;
	int m_sprite_sheet_frame_index;

	// Variables for Borderinteration
	bool m_border_collision_active_horizontal = false;
	bool m_border_collision_active_vertical	  = false;
	bool m_is_clone							  = false;

	// Variabels and Methods for Cannons & Projectiles
	std::vector<std::shared_ptr<Cannon>> m_cannon_container;
	std::shared_ptr<Player> m_shared_ptr_this;
	Player::FireMode m_fire_mode	   = Player::FireMode::InSequence;
	bool m_fire_sequence_ongoing	   = false;
	bool m_fire_sequence_ongoing_left  = false;
	bool m_fire_sequence_ongoing_right = false;
	float m_time_since_last_shot_left  = 0;
	float m_time_since_last_shot_right = 0;
	Player::FiringCannonIndex m_firing_cannon_index{1, 0};

	// Smear Variables
	Smear m_smear;
	Color m_smear_color = {9, 75, 101, 127};

	// invincibleity Variables
	bool m_can_be_hit		= true;
	bool m_is_invincible	= false;
	float m_iframe_timer	= 0;
	float m_iframe_duration = 5;

	// Sails
	std::shared_ptr<Sails> m_sails;

	// Loot Table
	LootTable m_loot_table;

	// Shader
	Shader m_flash_shader	 = LoadShader(NULL, "resources/shader/sprite_flash.fs");
	Vector4 m_flash_color	 = {255, 0, 0, 255};
	float m_flash_lerp_scale = 6;
	float m_flash_alpha		 = 0;
	int m_flash_alpha_location;

	// Upgrades
	std::unique_ptr<PSCore::Spawner<ExplosiveBarrel, AppLayer>> m_explosive_barrel_spawner;
	bool m_explosive_barrels_enabled = false;

	// Sound
	Sound m_hurt_sound	= LoadSound("resources/sfx/hurt.mp3");
	Sound m_death_sound = LoadSound("resources/sfx/death.mp3");

	float m_hurt_volume	 = 1;
	float m_hurt_pitch	 = 1;
	float m_death_volume = 1;
	float m_death_pitch	 = 1;

	Vector2 m_volume_boundary = {-15, 15};
	Vector2 m_pitch_boundary  = {-15, 15};
};

Player::Player() : PSInterfaces::IEntity("player")
{
	_p = std::make_unique<PlayerPriv>();

	Vector2 frame_grid{3, 4};
	_p->m_sprite = PRELOAD_TEXTURE(ident_, "resources/entity/player.png", frame_grid);

	_p->m_animation_controller = PSCore::sprites::SpriteSheetAnimation(
			_p->m_sprite->m_s_texture, {{2, 1, PSCore::sprites::KeyFrame, 1},
										{3, 1, PSCore::sprites::KeyFrame, 3},
										{3, 1, PSCore::sprites::KeyFrame, 3},
										{3, 1, PSCore::sprites::KeyFrame, 3},
										{10, 0.1, PSCore::sprites::Forward, 3}}
	);

	_p->m_animation_controller.add_animation_at_index(0, 1);
	_p->m_animation_controller.add_animation_at_index(2, 3);

	propose_z_index(0);

	// WARNING: THIS IS ONLY FOR TESTING
	if ( auto& vp = gApp()->viewport() ) {
		_p->m_position = vp->viewport_base_size() / 2;
	}

	auto director	   = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	auto& direcor_vals = director->value_container_ref();
	set_interpolation_values(6, 2, 4, direcor_vals->player_input_velocity_mult, direcor_vals->player_input_rotation_mult, 30);

	set_max_velocity(direcor_vals->player_max_velocity);
	_p->m_rotation		  = 0;
	_p->m_rotation_offset = 90;

	_p->m_sails = std::make_shared<Sails>(this);

	_p->m_sails->propose_z_index(20);
	_p->m_sails->set_is_active(true);

	if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
		app_layer->renderer()->submit_renderable(_p->m_sails);
	}

	// Hit Flash Shader
	SetShaderValue(_p->m_flash_shader, GetShaderLocation(_p->m_flash_shader, "flash_color"), &_p->m_flash_color, SHADER_UNIFORM_VEC4);
	_p->m_flash_alpha_location = GetShaderLocation(_p->m_flash_shader, "flash_alpha");
}

Player::~Player()
{
	UnloadShader(_p->m_flash_shader);

	UnloadSound(_p->m_hurt_sound);
	UnloadSound(_p->m_death_sound);
}

void Player::update(const float dt)
{
	if ( !_p->m_is_clone ) {
		// Input Functions to set Target Velocity and Target Rotation
		if ( IsKeyDown(std::get<int>(PSCore::SettingsManager::inst()->settings.at("user_preferences")->value("key_accelerate").value_or(KEY_W))) ) {
			_p->m_target_velocity += _p->m_target_velocity < _p->m_max_velocity ? _p->m_input_velocity_multiplier * dt : 0;
		}
		if ( IsKeyDown(std::get<int>(PSCore::SettingsManager::inst()->settings.at("user_preferences")->value("key_brake").value_or(KEY_S))) ) {
			_p->m_target_velocity -= _p->m_target_velocity > 0 ? _p->m_input_velocity_multiplier * dt : 0;
		}
		if ( IsKeyDown(std::get<int>(PSCore::SettingsManager::inst()->settings.at("user_preferences")->value("key_right_turn").value_or(KEY_D))) &&
			 Vector2Length(_p->m_velocity) - (_p->m_velocity_rotation_downscale * fabsf(_p->m_rotation_velocity)) > CALCULATION_VELOCITY_MIN ) {
			_p->m_target_rotation += _p->m_input_rotation_multiplier * dt;
		}
		if ( IsKeyDown(std::get<int>(PSCore::SettingsManager::inst()->settings.at("user_preferences")->value("key_left_turn").value_or(KEY_A))) &&
			 Vector2Length(_p->m_velocity) - (_p->m_velocity_rotation_downscale * fabsf(_p->m_rotation_velocity)) > CALCULATION_VELOCITY_MIN ) {
			_p->m_target_rotation -= _p->m_input_rotation_multiplier * dt;
		}
	}

	if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
		if ( director->player_health() > 0 ) {
			fire_cannons(dt);

			calculate_movement(dt);

			_p->m_rotation_velocity = calculate_rotation_velocity(0.01, dt);

			reset_iframe(dt);

			// Animation Calculation

			if ( _p->m_animation_controller.get_sprite_sheet_animation_index(3).value_or(2) == 2 &&
				 fabsf(_p->m_rotation_velocity) > SCHMITT_TRIGGER_DELTA_ROTATION_MAX ) {
				_p->m_sprite_sheet_animation_index = _p->m_rotation_velocity < 0 ? 1 : 3;
			}
			if ( _p->m_animation_controller.get_sprite_sheet_animation_index(3).value_or(2) != 2 &&
				 fabsf(_p->m_rotation_velocity) < SCHMITT_TRIGGER_DELTA_ROTATION_MIN ) {
				_p->m_sprite_sheet_animation_index = 2;
			}

			_p->m_sprite_sheet_frame_index = static_cast<int>(round((Vector2Length(_p->m_velocity) / _p->m_max_velocity) * 2));

			_p->m_animation_controller.set_animation_at_index(_p->m_sprite_sheet_animation_index, _p->m_sprite_sheet_frame_index, 3);
		} else {
			_p->m_target_velocity = 0;
			_p->m_velocity		  = {0, 0};

			if ( _p->m_animation_controller.get_sprite_sheet_frame_index(3) == 9 ) {
				set_is_active(false);
				_p->m_sails->set_is_active(false);
			}
		}
	}

	_p->m_animation_controller.update_animation(dt);

	// Smear Calculation

	_p->m_smear.update_smear(_p->m_rotation - _p->m_target_rotation, 0.5, 10, dt);

	// m_smear.update_smear(m_rotation_velocity, -10, 10, dt);

	if ( auto& vp = gApp()->viewport() ) {
		Vector2 position_absolute = vp->position_viewport_to_global(_p->m_position);
		float scale				  = vp->viewport_scale();
		Vector2 smear_right_position =
				coordinatesystem::point_relative_to_global_leftup(position_absolute, _p->m_rotation, Vector2Scale({18, 5}, scale));
		Vector2 smear_left_position =
				coordinatesystem::point_relative_to_global_leftdown(position_absolute, _p->m_rotation, Vector2Scale({18, 5}, scale));
		Vector2 smear_forward_position =
				coordinatesystem::point_relative_to_global_rightup(position_absolute, _p->m_rotation, Vector2Scale({20, 0}, scale));

		_p->m_smear.calculate_linear_smear(smear_right_position, Vector2Length(_p->m_velocity), _p->m_rotation, 0.15f * scale, 0, 0);
		_p->m_smear.calculate_linear_smear(smear_left_position, Vector2Length(_p->m_velocity), _p->m_rotation, 0.15f * scale, 0, 1);
		_p->m_smear.calculate_exponential_smear(
				smear_forward_position, Vector2Length(_p->m_velocity), _p->m_rotation, 0.15f * scale, 0, 0.03f * scale, 0.05f * scale, 2
		);
		_p->m_smear.calculate_exponential_smear(
				smear_forward_position, Vector2Length(_p->m_velocity), _p->m_rotation, 0.15f * scale, 0, -0.03f * scale, -0.05f * scale, 3
		);
		_p->m_smear.add_smear_wave(0.1, 0.25, Vector2Length(_p->m_velocity), _p->m_max_velocity, dt, 0);

		_p->m_smear.update_smear_wave({0, 1}, Linear, 1, 10, Vector2Length(_p->m_velocity), _p->m_max_velocity, dt);
	}

	// Barrel
	if ( _p->m_explosive_barrel_spawner ) {
		if ( !CFG_VALUE<bool>("player_spawn_explosive_barrel_when_idle", false) && Vector2Length(_p->m_velocity) < CALCULATION_VELOCITY_MIN )
			_p->m_explosive_barrel_spawner->suspend();
		else
			_p->m_explosive_barrel_spawner->resume();

		_p->m_explosive_barrel_spawner->update(dt);
	}
}

void Player::on_hit()
{
	if ( _p->m_can_be_hit && !_p->m_is_invincible ) {
		_p->m_can_be_hit = false;
		if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
			director->set_player_health(director->player_health() - 1);
			play_sound(_p->m_hurt_sound, _p->m_hurt_volume, _p->m_hurt_pitch);
			if ( director->player_health() <= 0 ) {
				on_death();
			}
		}
	}
}

void Player::on_death()
{
	if ( gApp()->get_layer<ScoreLayer>() ) {
		gApp()->pop_layer<ScoreLayer>();
	}
	if ( gApp()->get_layer<DeathScreenLayer>() ) {
		gApp()->pop_layer<DeathScreenLayer>();
	}
	if ( gApp()->get_layer<UILayer>() ) {
		gApp()->pop_layer<UILayer>();
	}

	gApp()->push_layer<ScoreLayer>();
	auto score_layer = gApp()->get_layer<ScoreLayer>();
	if ( score_layer ) {
		score_layer->set_layer_is_visible(false);
		score_layer->reset_state();
		score_layer->load_highscore(score_layer->score_filename());
		score_layer->save_new_highscore(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
		score_layer->player_name_input = gApp()->current_player_name();
	}

	for ( const auto& cannon: _p->m_cannon_container ) {
		cannon->set_is_active(false);
	}

	_p->m_animation_controller.set_animation_at_index(0, 1, 1);
	_p->m_animation_controller.set_animation_at_index(4, 0, 3);

	play_sound(_p->m_death_sound, _p->m_death_volume, _p->m_death_pitch);

	gApp()->get_layer<AppLayer>()->set_can_open_pause_menu(false);
	gApp()->push_layer<DeathScreenLayer>();
	auto death_layer = gApp()->get_layer<DeathScreenLayer>();
	if ( death_layer && score_layer ) {
		death_layer->set_score_layer_instance(score_layer);
		death_layer->set_score_should_be_saved(
				score_layer->check_for_new_highscore(dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty())
		);
	}
	ShowCursor();
}

void Player::set_is_invincible(bool invincible)
{
	_p->m_is_invincible = invincible;
}

void Player::fire_cannons(float dt)
{
	int shoot_left_key = std::get<int>(PSCore::SettingsManager::inst()->settings.at("user_preferences")->value("key_left_shoot").value_or(KEY_LEFT));
	int shoot_right_key =
			std::get<int>(PSCore::SettingsManager::inst()->settings.at("user_preferences")->value("key_right_shoot").value_or(KEY_RIGHT));
	int shoot_all_key = std::get<int>(PSCore::SettingsManager::inst()->settings.at("user_preferences")->value("key_all_shoot").value_or(KEY_SPACE));

	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( director ) {
		switch ( _p->m_fire_mode ) {

			case FireMode::InSequence: {
				_p->m_time_since_last_shot_left += dt;
				_p->m_time_since_last_shot_right += dt;
				if ( IsKeyDown(shoot_all_key) && !_p->m_fire_sequence_ongoing ) {
					_p->m_fire_sequence_ongoing		  = true;
					_p->m_fire_sequence_ongoing_right = true;
					_p->m_fire_sequence_ongoing_left  = true;
				}

				if ( (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsKeyDown(shoot_right_key)) && !_p->m_fire_sequence_ongoing &&
					 !_p->m_fire_sequence_ongoing_right ) {
					_p->m_fire_sequence_ongoing_right = true;
				}
				if ( _p->m_time_since_last_shot_right > _p->m_cannon_container.at(0)->fire_rate() / _p->m_cannon_container.size() &&
					 _p->m_fire_sequence_ongoing_right ) {
					_p->m_cannon_container.at(_p->m_firing_cannon_index.right)->fire(director->player_projectile_amount());

					_p->m_firing_cannon_index.right += 2;
					_p->m_time_since_last_shot_right = 0;

					if ( _p->m_firing_cannon_index.right >= _p->m_cannon_container.size() ) {
						_p->m_firing_cannon_index.right	  = 1;
						_p->m_fire_sequence_ongoing_right = false;
						_p->m_fire_sequence_ongoing		  = false;
					}
				}
				if ( (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(shoot_left_key)) && !_p->m_fire_sequence_ongoing &&
					 !_p->m_fire_sequence_ongoing_left ) {
					_p->m_fire_sequence_ongoing_left = true;
				}

				if ( _p->m_time_since_last_shot_left > _p->m_cannon_container.at(0)->fire_rate() / _p->m_cannon_container.size() &&
					 _p->m_fire_sequence_ongoing_left ) {
					_p->m_cannon_container.at(_p->m_firing_cannon_index.left)->fire(director->player_projectile_amount());
					_p->m_firing_cannon_index.left += 2;
					_p->m_time_since_last_shot_left = 0;
					if ( _p->m_firing_cannon_index.left >= _p->m_cannon_container.size() ) {
						_p->m_firing_cannon_index.left	 = 0;
						_p->m_fire_sequence_ongoing_left = false;
						_p->m_fire_sequence_ongoing		 = false;
					}
				}
				break;
			}

			case FireMode::SameTime: {
				if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(shoot_left_key) ) {
					for ( auto cannon: _p->m_cannon_container ) {
						if ( cannon->positioning() == Cannon::CannonPositioning::Left ) {
							cannon->fire(director->player_projectile_amount());
						}
					}
				}
				if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsKeyDown(shoot_right_key) ) {
					for ( auto cannon: _p->m_cannon_container ) {
						if ( cannon->positioning() == Cannon::CannonPositioning::Right ) {
							cannon->fire(director->player_projectile_amount());
						}
					}
				}
				if ( IsKeyDown(shoot_all_key) ) {
					for ( auto cannon: _p->m_cannon_container ) {
						cannon->fire(director->player_projectile_amount());
					}
					break;
				}
			}
		}
	}
}

void Player::render()
{
	// Draw Smear

	if ( Vector2Length(_p->m_velocity) > CALCULATION_VELOCITY_MIN ) {
		if ( auto& vp = gApp()->viewport() ) {
			_p->m_smear.draw_smear(0, Linear, 2 * vp->viewport_scale(), 1, _p->m_smear_color);
			_p->m_smear.draw_smear(1, Linear, 2 * vp->viewport_scale(), 1, _p->m_smear_color);
			_p->m_smear.draw_smear(2, Exponential, 2 * vp->viewport_scale(), 1, _p->m_smear_color);
			_p->m_smear.draw_smear(3, Exponential, 2 * vp->viewport_scale(), 1, _p->m_smear_color);
			_p->m_smear.draw_smear_wave(Vector2Length(_p->m_velocity), _p->m_max_velocity, 2 * vp->viewport_scale(), 1, _p->m_smear_color);
		}
	}

	// Draw Ship

	BeginShaderMode(_p->m_flash_shader);

	if ( auto& vp = gApp()->viewport() ) {
		vp->draw_in_viewport(
				_p->m_sprite->m_s_texture, _p->m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}), _p->m_position,
				_p->m_rotation + _p->m_rotation_offset, WHITE
		);
	}

	EndShaderMode();
}

void Player::reset_iframe(float dt)
{
	if ( !_p->m_can_be_hit ) {
		if ( auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director()) ) {
			_p->m_iframe_timer += dt;
			if ( _p->m_iframe_timer >= director->player_iframe_duration() ) {
				_p->m_can_be_hit   = true;
				_p->m_iframe_timer = 0;
			}
			float n_flash_lerp_scale = _p->m_flash_lerp_scale / director->player_iframe_duration();
			_p->m_flash_alpha = 0.5f * cosf(_p->m_iframe_timer * sqrtf(_p->m_iframe_timer) * n_flash_lerp_scale * sqrtf(n_flash_lerp_scale)) + 0.5;
		}
	} else {
		_p->m_flash_alpha = 0;
	}

	SetShaderValue(_p->m_flash_shader, _p->m_flash_alpha_location, &_p->m_flash_alpha, SHADER_UNIFORM_FLOAT);
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
	return _p->m_position;
}

float Player::target_velocity()
{
	return _p->m_target_velocity;
}

float Player::max_velocity()
{
	return _p->m_max_velocity;
}

float Player::target_rotation()
{
	return _p->m_target_rotation;
}

void Player::set_position(const Vector2& position)
{
	_p->m_position = position;
}

void Player::set_velocity(const Vector2& velocity)
{
	// Check if the input Velocity is in range of the Max Velocity
	// and set the Velocity and Target Velocity to the input Velocity if the check is True
	// or if the check is False it's set to Max Velocity
	_p->m_velocity		  = Vector2Length(velocity) <= _p->m_max_velocity ? velocity : Vector2Scale(Vector2Normalize(velocity), _p->m_max_velocity);
	_p->m_target_velocity = Vector2Length(velocity) <= _p->m_max_velocity ? Vector2Length(velocity) : _p->m_max_velocity;
}

void Player::set_target_velocity(const float target_velocity)
{
	_p->m_target_velocity = target_velocity;
}

void Player::set_max_velocity(const float max_velocity)
{
	_p->m_max_velocity = max_velocity;
}

void Player::set_rotation(const float rotation)
{
	_p->m_rotation		  = rotation;
	_p->m_target_rotation = rotation;
}

void Player::set_target_rotation(const float target_rotation)
{
	_p->m_target_rotation = target_rotation;
}

void Player::set_interpolation_values(
		const float acceleration_fade, const float deceleration_fade, const float rotation_fade, const float input_velocity_multiplier,
		const float input_rotation_multiplier, const float velocity_rotation_downscale
)
{
	_p->m_acceleration_fade			  = acceleration_fade;
	_p->m_deceleration_fade			  = deceleration_fade;
	_p->m_rotation_fade				  = rotation_fade;
	_p->m_input_velocity_multiplier	  = input_velocity_multiplier;
	_p->m_input_rotation_multiplier	  = input_rotation_multiplier;
	_p->m_velocity_rotation_downscale = velocity_rotation_downscale;
}

void Player::apply_repel_force(Vector2 repel_force)
{
	_p->m_repel_velocity = Vector2Add(_p->m_repel_velocity, repel_force);
}

void Player::calculate_movement(const float dt)
{
	// Linear Interpolation form Rotation to Target Rotation with a regression of Rotation and a static Alpha
	// which ends in an exponential approximation to calculate the rotation
	_p->m_rotation = _p->m_rotation + (_p->m_target_rotation - _p->m_rotation) * _p->m_rotation_fade * dt;

	// Check if the Velocity should increase or decrease and uses right the Linear Interpolation form Velocity to Target Velocity with a regression of
	// Velocity and a static Alpha which ends in an exponential approximation to calculate the Value of the Velocity
	float velocity_value = (_p->m_target_velocity - Vector2Length(_p->m_velocity)) > 0
								   ? Vector2Length(_p->m_velocity) + (_p->m_target_velocity - Vector2Length(_p->m_velocity) -
																	  (_p->m_velocity_rotation_downscale * fabsf(_p->m_rotation_velocity))) *
																			 _p->m_acceleration_fade * dt
								   : Vector2Length(_p->m_velocity) + (_p->m_target_velocity - Vector2Length(_p->m_velocity) -
																	  (_p->m_velocity_rotation_downscale * fabsf(_p->m_rotation_velocity))) *
																			 _p->m_deceleration_fade * dt;

	// Calculate with the Velocity Value and the Rotation the actual 2 Dimensional Velocity
	_p->m_velocity.x = velocity_value * cos(_p->m_rotation * DEG2RAD);
	_p->m_velocity.y = velocity_value * sin(_p->m_rotation * DEG2RAD);

	// Update Position based on Velocity
	_p->m_position.x += (_p->m_velocity.x + _p->m_repel_velocity.x) * dt;
	_p->m_position.y += (_p->m_velocity.y + _p->m_repel_velocity.y) * dt;

	_p->m_repel_velocity = Vector2Subtract(_p->m_repel_velocity, Vector2Scale(_p->m_repel_velocity, 2.0f * dt));
}

float Player::calculate_rotation_velocity(float frequency, float dt)
{
	static float timer			   = 0;
	static float last_rotation	   = 0;
	static float rotation_velocity = 0;

	timer += dt;

	if ( timer >= frequency ) {
		timer = 0;

		rotation_velocity = _p->m_rotation - last_rotation;

		last_rotation = _p->m_rotation;
	}

	return rotation_velocity;
}

void Player::initialize_cannon()
{
	auto director = dynamic_cast<FortunaDirector*>(gApp()->game_director());
	if ( !director ) {
		return;
	}

	float cannon_width = 5.0f;

	float x_offset = 0;
	if ( !_p->m_cannon_container.empty() ) {
		cannon_width = static_cast<float>((float)_p->m_cannon_container[0]->texture().width / 7);
	}
	x_offset = -(((cannon_width / 4)) * _p->m_cannon_container.size()) / 2;

	for ( int i = 0; i < 2; i++ ) {
		auto new_cannon = director->spawn_cannon(_p->m_position);
		_p->m_cannon_container.push_back(new_cannon);
		new_cannon->set_parent(_p->m_shared_ptr_this);
		new_cannon->set_parent_position_x_offset(x_offset);
		new_cannon->set_parent_position_y_offset((float)new_cannon->texture().height / 3);
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
	_p->m_border_collision_active_horizontal = active;
}

bool Player::border_collision_active_horizontal() const
{
	return _p->m_border_collision_active_horizontal;
}

void Player::set_border_collision_active_vertical(bool active)
{
	_p->m_border_collision_active_vertical = active;
}

bool Player::border_collision_active_vertical() const
{
	return _p->m_border_collision_active_vertical;
}

void Player::set_is_clone(bool active)
{
	_p->m_is_clone = active;
}

bool Player::is_clone() const
{
	return _p->m_is_clone;
}

std::vector<std::shared_ptr<Cannon>>& Player::cannon_container()
{
	return _p->m_cannon_container;
}

void Player::set_cannon_container(const std::vector<std::shared_ptr<Cannon>>& container)
{
	_p->m_cannon_container = container;
}

std::shared_ptr<Player> Player::shared_ptr_this()
{
	return _p->m_shared_ptr_this;
}

void Player::set_shared_ptr_this(std::shared_ptr<Player> ptr)
{
	_p->m_shared_ptr_this = ptr;
}

std::optional<std::vector<Vector2>> Player::bounds() const
{
	if ( is_active() )
		if ( auto& vp = gApp()->viewport() ) {

			Vector2 vp_pos = vp->position_viewport_to_global(_p->m_position);
			float scale	   = vp->viewport_scale();

			std::vector<Vector2> hitbox_points = {{20 * scale, 0 * scale},	{8 * scale, 6 * scale},	   {-15 * scale, 6 * scale},
												  {-20 * scale, 0 * scale}, {-15 * scale, -6 * scale}, {8 * scale, -6 * scale}};

			return coordinatesystem::points_relative_to_globle_rightup(vp_pos, _p->m_rotation, hitbox_points);
		}

	return std::nullopt;
}
void Player::set_input_velocity_multiplier(float val)
{
	_p->m_input_velocity_multiplier = val;
};
void Player::set_input_rotation_multiplier(float val)
{
	_p->m_input_rotation_multiplier = val;
};

Player::FireMode Player::fire_mode() const
{
	return _p->m_fire_mode;
}

void Player::set_fire_mode(FireMode mode)
{
	_p->m_fire_mode = mode;
}

//
// Player Sails
//
Sails::Sails(Player* player) : PSInterfaces::IEntity("player_sails"), m_player(player)
{
	Vector2 frame_grid{3, 4};
	m_sprite = PRELOAD_TEXTURE(ident_, "resources/emissive/player_emissive.png", frame_grid);

	if ( PSCore::SettingsManager::inst()->settings.find("user_preferences") != PSCore::SettingsManager::inst()->settings.end() ) {
		auto& settings	 = PSCore::SettingsManager::inst()->settings["user_preferences"];
		m_emissive_color = PSUtils::color_to_vector3(std::get<int>(settings->value("player_color").value_or(0xFFFFFF)));
	}

	m_emissive_texture_location = GetShaderLocation(m_emissive_shader, "texture_emissive");
	SetShaderValue(m_emissive_shader, GetShaderLocation(m_emissive_shader, "emissive_color"), &m_emissive_color, SHADER_UNIFORM_VEC3);
}

Sails::~Sails()
{
	UnloadShader(m_emissive_shader);
}

void Sails::update(float dt)
{
}

void Sails::render()
{
	BeginShaderMode(m_emissive_shader);

	SetShaderValueTexture(m_emissive_shader, m_emissive_texture_location, m_sprite->m_s_texture);

	if ( auto& vp = gApp()->viewport() ) {
		auto texture = m_player->_p->m_sprite;
		vp->draw_in_viewport(
				texture->m_s_texture, m_player->_p->m_animation_controller.get_source_rectangle(3).value_or(Rectangle{0}), m_player->_p->m_position,
				m_player->_p->m_rotation + m_player->_p->m_rotation_offset, WHITE
		);
	}

	EndShaderMode();
}

void Sails::draw_debug()
{
}

std::optional<float> Player::rotation() const
{
	return _p->m_rotation;
}

std::optional<Vector2> Player::velocity() const
{
	return _p->m_velocity;
}

std::optional<Vector2> Player::size() const
{
	auto h = _p->m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}).height;
	auto w = _p->m_animation_controller.get_source_rectangle(1).value_or(Rectangle{0}).width;
	return Vector2{w, h};
}

void Player::enable_explosive_barrels()
{
	if ( _p->m_explosive_barrel_spawner )
		return;

	_p->m_explosive_barrel_spawner =
			std::make_unique<PSCore::Spawner<ExplosiveBarrel, AppLayer>>(CFG_VALUE<float>("explosive_barrel_intervall", 5.f), 0, INT32_MAX, true);
	_p->m_explosive_barrel_spawner->register_spawn_callback([this](std::shared_ptr<ExplosiveBarrel> barrel) {
		barrel->init(barrel, _p->m_position);
		barrel->set_parent(shared_ptr_this());
	});
	_p->m_explosive_barrel_spawner->resume();
	_p->m_explosive_barrels_enabled = true;
};

void Player::set_barrel_intervall(float interval)
{
	_p->m_explosive_barrel_spawner->set_interval(interval);
};

void Player::play_sound(Sound& sound, float volume, float pitch)
{
	int random_volume = PSUtils::gen_rand(_p->m_volume_boundary.x, _p->m_volume_boundary.y);
	int random_pitch  = PSUtils::gen_rand(_p->m_pitch_boundary.x, _p->m_pitch_boundary.y);

	float global_sfx_volume = gApp()->sound_volume(PSCore::Application::SoundType::SFX).value_or(50);

	SetSoundVolume(sound, std::min((global_sfx_volume / 100) * (volume + static_cast<float>(random_volume) / 100), 1.0f));
	SetSoundPitch(sound, pitch + static_cast<float>(random_pitch) / 100);

	PlaySound(sound);
}

bool Player::explosive_barrels_enabled() const
{
	return _p->m_explosive_barrels_enabled;
}
