#pragma once

#include <raylib.h>
#include <vector>

#ifndef SPLINE_SEGMENT_DIVISIONS
#define SPLINE_SEGMENT_DIVISIONS 24 // Spline segment divisions
#endif

#ifndef WAVE_VECTOR_SIZE_MIN
#define WAVE_VECTOR_SIZE_MIN 10 // The minimum size of the Vector for Wave Calculation
#endif

namespace smear {
	struct SmearPoints
	{
		Vector2 p1;
		Vector2 c2;
		Vector2 c3;
		Vector2 p4;
	};

	enum UseSmearPointsForWaveCalculation { linear_points, exponential_points };

	/**
	 * @brief Updates the given smear_rotation to the given Value
	 * @warning Call it once per Frame at the Actor
	 * @param smear_rotation A Pointer to a Variable with the Smear Rotation
	 * @param actor_rotation_velocity The Rotation Velocity or the difference between Rotation and Target Rotation
	 * @param deflection_scale A Scalar for actor_rotation_velocity
	 * @param deflection_velocity The Velocity with the Rotation interpolates to actor_rotation_velocity * deflection_scale
	 * @param delta_time Delta Time
	 */
	void
	update_smear_rotation(float* smear_rotation, float actor_rotation_velocity, float deflection_scale, float deflection_velocity, float delta_time);

	/**
	 * @brief Calculates the Points for a Linear Smear
	 * @param position_start Position where the Smear starts in the global Coordinate system
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_rotation The Rotation of the Actor
	 * @param smear_rotation The Calculated Smear Rotation
	 * @param smear_length The Length multiplier of the Smear
	 * @param smear_rotation_offset The default Rotation of the Smear
	 * @return The Points for the Smear to Draw
	 */
	SmearPoints calculate_smear_linear_points(
			Vector2& position_start, float actor_velocity, float actor_rotation, float smear_rotation, float smear_length, float smear_rotation_offset
	);

	/**
	 * @brief Draws a Linear Smear on the given Values
	 * @param smear_points The Points of the Smear
	 * @param smear_line_thickness The Thickness of the Drawn Smear
	 * @param smear_line_thickness_falloff The Thickness Falloff of the Smear over the Length
	 * @param smear_color The Color of the Smear
	 */
	void draw_smear_linear(SmearPoints& smear_points, float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color);

	/**
	 * @brief Calculates the Points for an Exponential Smear
	 * @param position_start Position where the Smear starts in the global Coordinate system
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_rotation The Rotation of the Actor
	 * @param smear_rotation The Calculated Smear Rotation
	 * @param smear_length The Length multiplier of the Smear
	 * @param smear_rotation_offset The default Rotation of the Smear
	 * @param smear_deflection_start The Deflection of the Exponential Curve at the start
	 * @param smear_deflection_length The Deflection of the Exponential Curve at the Half length
	 * @return The Points for the Smear to Draw
	 */
	SmearPoints calculate_smear_exponential_points(
			Vector2& position_start, float actor_velocity, float actor_rotation, float smear_rotation, float smear_length,
			float smear_rotation_offset, float smear_deflection_start, float smear_deflection_length
	);

	/**
	 * @brief Draws an Exponential Smear on the given Values
	 * @param smear_points The Points of the Smear
	 * @param smear_line_thickness The Thickness of the Drawn Smear
	 * @param smear_line_thickness_falloff The Thickness Falloff of the Smear over the Length
	 * @param smear_color The Color of the Smear
	 */
	void draw_smear_exponential(SmearPoints& smear_points, float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color);

	/**
	 * @brief Sends a new Wave at the start Location of the Smear
	 * @param smear_waves A Pointer to a Vector of floats with Values between 0 and 1 which is the Location of the Wave on the Smear
	 * @param smear_wave_index A Pointer to an Index of the smear_waves Vector
	 * @param smear_wave_velocity The Velocity of the Wave to move
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_max_velocity The max Velocity of the Actor
	 * @param delta_time Delta Time
	 */
	void send_smear_wave(
			std::vector<float>* smear_waves, int* smear_wave_index, float smear_wave_velocity, float actor_velocity, float actor_max_velocity,
			float delta_time
	);

	/**
	 * @brief Sends a new Wave to the smear_wave_start Value
	 * @param smear_waves A Pointer to a Vector of floats with Values between 0 and 1 which is the Location of the Wave on the Smear
	 * @param smear_wave_index A Pointer to an Index of the smear_waves Vector
	 * @param smear_wave_start The start Location of the Smear Wave
	 */
	void send_smear_wave_at_start(std::vector<float>* smear_waves, int* smear_wave_index, float smear_wave_start);

	/**
	 * @brief Updates the Location of the Smear Waves
	 * @param smear_waves A Pointer to a Vector of floats with Values between 0 and 1 which is the Location of the Wave on the Smear
	 * @param smear_wave_velocity The Velocity of the Wave to move
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_max_velocity The max Velocity of the Actor
	 * @param delta_time Delta Time
	 */
	void
	update_smear_wave(std::vector<float>* smear_waves, float smear_wave_velocity, float actor_velocity, float actor_max_velocity, float delta_time);

	/**
	 * @brief Calculate the Points for the Smear Wave
	 * @param smear_wave_points A Pointer to a Vector of SmearPoints that contains the Points to draw the Waves
	 * @param smear_waves A Reference to a Vector of floats with Values between 0 and 1 which is the Location of the Wave on the Smear
	 * @param smear_points A Reference to a Vector of SmearPoints the contains the Points of the Smears for the Smear Wave
	 * @param smear_points_vector_index A Vector2 with the 2 Indexes in the smear_points Vector to use
	 * @param smear_rotation The Calculated Smear Rotation
	 * @param smear_wave_height The Height of the Smear Wave (Amplitude)
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_max_velocity The max Velocity of the Actor
	 * @param smear_type_of_smear_points An Enum that contains the type of the Smears at both Indexes
	 */
	void calculate_smear_wave_points(
			std::vector<SmearPoints>* smear_wave_points, std::vector<float>& smear_waves, std::vector<SmearPoints>& smear_points,
			Vector2 smear_points_vector_index, float smear_rotation, float smear_wave_height, float actor_velocity, float actor_max_velocity,
			UseSmearPointsForWaveCalculation smear_type_of_smear_points
	);

	/**
	 * @brief Draw the Smear Waves between 2 Smears
	 * @param smear_wave_points A Reference to a Vector of SmearPoints that are the Points of the Smear Wave
	 * @param smear_waves A Reference to a Vector of floats with Values between 0 and 1 which is the Location of the Wave on the Smear
	 * @param actor_velocity The Velocity of the Actor
	 * @param actor_max_velocity The max Velocity of the Actor
	 * @param smear_wave_line_thickness The Thickness of the Drawn Smear Wave
	 * @param smear_wave_line_thickness_falloff The Thickness Falloff of the Smear Wave over the Length
	 * @param smear_wave_color The Color of the Smear Wave
	 */
	void draw_smear_wave_between_smears(
			std::vector<SmearPoints>& smear_wave_points, std::vector<float>& smear_waves, float actor_velocity, float actor_max_velocity,
			float smear_wave_line_thickness, float smear_wave_line_thickness_falloff, Color smear_wave_color
	);
}; // namespace smear
