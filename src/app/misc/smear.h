#pragma once

#include <raylib.h>
#include <vector>

#ifndef SPLINE_SEGMENT_DIVISIONS
#define SPLINE_SEGMENT_DIVISIONS 24 // Spline segment divisions
#endif

#ifndef WAVE_VECTOR_SIZE_MIN
#define WAVE_VECTOR_SIZE_MIN 10 // The minimum size of the Vector for Wave Calculation
#endif

struct SmearPoints
{
	Vector2 p1;
	Vector2 c2;
	Vector2 c3;
	Vector2 p4;
};

enum SmearType { Linear, Exponential };

class Smear
{
public:
	/**
	 * @brief Updates the given smear_rotation to the given Value
	 * @warning Call it once per Frame at the Actor
	 * @param actor_rotation_velocity The Rotation Velocity or the difference between Rotation and Target Rotation
	 * @param deflection_scale A Scalar for actor_rotation_velocity
	 * @param deflection_velocity The Velocity with the Rotation interpolates to actor_rotation_velocity * deflection_scale
	 * @param delta_time Delta Time
	 */
	void update_smear(float actor_rotation_velocity, float deflection_scale, float deflection_velocity, float delta_time);

	/**
	 * @brief Calculates the Points for a Linear Smear
	 * @param start_position Position where the Smear starts in the global Coordinate system
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_rotation The Rotation of the Actor
	 * @param smear_length The Length multiplier of the Smear
	 * @param smear_rotation_offset The default Rotation of the Smear
	 * @param smear_index The Index for the Smear Vector to save the Points
	 */
	void calculate_linear_smear(
			Vector2& start_position, float actor_velocity, float actor_rotation, float smear_length, float smear_rotation_offset, int smear_index
	);

	/**
	 * @brief Calculates the Points for an Exponential Smear
	 * @param start_position Position where the Smear starts in the global Coordinate system
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_rotation The Rotation of the Actor
	 * @param smear_length The Length multiplier of the Smear
	 * @param smear_rotation_offset The default Rotation of the Smear
	 * @param smear_deflection_start The Deflection of the Exponential Curve at the start
	 * @param smear_deflection_length The Deflection of the Exponential Curve at the Half length
	 * @param smear_index The Index for the Smear Vector to save the Points
	 */
	void calculate_exponential_smear(
			Vector2& start_position, float actor_velocity, float actor_rotation, float smear_length, float smear_rotation_offset,
			float smear_deflection_start, float smear_deflection_length, int smear_index
	);

	/**
	 * @brief Draws a Smear on the given Values
	 * @param smear_index The Index of the Smear Points in the Vector
	 * @param smear_type The type of Smear to Draw
	 * @param smear_line_thickness The Thickness of the Drawn Smear
	 * @param smear_line_thickness_falloff The Thickness Falloff of the Smear over the Length
	 * @param smear_color The Color of the Smear
	 */
	void draw_smear(int smear_index, SmearType smear_type, float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color);

	/**
	 * @brief Updates the Location of the Smear Waves
	 * @param smear_indices The Indices of the Smear Points in the Vector for the Wave
	 * @param smear_type The type of Smear Points in the Vector at the Indices
	 * @param smear_wave_velocity The Velocity of the Wave to move
	 * @param smear_wave_height The Height of the Smear Wave (Amplitude)
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_max_velocity The max Velocity of the Actor
	 * @param delta_time Delta Time
	 */
	void update_smear_wave(
			Vector2 smear_indices, SmearType smear_type, float smear_wave_velocity, float smear_wave_height, float actor_velocity,
			float actor_max_velocity, float delta_time
	);

	/**
	 * @brief Sends a new Wave at the start Location of the Smear
	 * @param smear_wave_velocity The Velocity of the Wave to move
	 * @param smear_wave_frequency The Frequency in that the Waves spawn
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_max_velocity The max Velocity of the Actor
	 * @param delta_time Delta Time
	 * @param smear_timer_index The index in that the current Timer Value is saved
	 */
	void add_smear_wave(
			float smear_wave_velocity, float smear_wave_frequency, float actor_velocity, float actor_max_velocity, float delta_time,
			int smear_timer_index
	);

	/**
	 * @brief Sends a new Wave to the smear_wave_start Value
	 * @param smear_wave_start The start Location of the Wave
	 * @param smear_wave_frequency The Frequency in that the Waves spawn
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_max_velocity The max Velocity of the Actor
	 * @param delta_time Delta Time
	 * @param smear_timer_index The index in that the current Timer Value is saved
	 */
	void add_smear_wave_at_location(
			float smear_wave_start, float smear_wave_frequency, float actor_velocity, float actor_max_velocity, float delta_time,
			int smear_timer_index
	);

	/**
	 * @brief Draw the Smear Waves between 2 Smears
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_max_velocity The max Velocity of the Actor
	 * @param smear_wave_line_thickness The Thickness of the Drawn Smear Wave
	 * @param smear_wave_line_thickness_falloff The Thickness Falloff of the Smear Wave over the Length
	 * @param smear_wave_color The Color of the Smear Wave
	 */
	void draw_smear_wave(
			float actor_velocity, float actor_max_velocity, float smear_wave_line_thickness, float smear_wave_line_thickness_falloff,
			Color smear_wave_color
	);

private:
	float m_smear_rotation						 = 0;
	std::vector<SmearPoints> m_smear_points		 = {{0}};
	int m_smear_wave_index						 = 0;
	std::vector<float> m_smear_wave				 = {0};
	std::vector<float> m_smear_wave_timer		 = {0};
	std::vector<SmearPoints> m_smear_wave_points = {{0}};
};