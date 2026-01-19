#pragma once

#include "raylib.h"

#ifndef SPLINE_SEGMENT_DIVISIONS
#define SPLINE_SEGMENT_DIVISIONS 24 // Spline segment divisions
#endif

namespace smear {
	struct SmearPoints
	{
		Vector2 p1;
		Vector2 c2;
		Vector2 c3;
		Vector2 p4;
	};

	/**
	 * @brief Updates the given smear_rotation to the given Value. Call it once per Frame at the Actor
	 * @param smear_rotation A Reference to a Variable with the Smear Rotation
	 * @param actor_rotation_velocity The Rotation Velocity or the difference between Rotation and Target Rotation
	 * @param deflection_scale A Scalar for actor_rotation_velocity
	 * @param deflection_velocity The Velocity with the Rotation interpolates to actor_rotation_velocity * deflection_scale
	 * @param delta_time
	 */
	void
	update_smear_rotation(float& smear_rotation, float actor_rotation_velocity, float deflection_scale, float deflection_velocity, float delta_time);

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
}; // namespace smear
