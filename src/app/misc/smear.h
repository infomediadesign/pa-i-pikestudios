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

	void
	update_smear_rotation(float& smear_rotation, float actor_rotation_velocity, float deflection_scale, float deflection_velocity, float delta_time);

	SmearPoints calculate_smear_linear_points(
			Vector2& position_start, float actor_velocity, float actor_rotation, float smear_rotation, float smear_length, float smear_rotation_offset
	);

	void draw_smear_linear(SmearPoints& smear_points, float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color);

	SmearPoints calculate_smear_exponential_points(
			Vector2& position_start, float actor_velocity, float actor_rotation, float smear_rotation, float smear_length,
			float smear_rotation_offset, float smear_deflection_start, float smear_deflection_length
	);

	void
	draw_smear_exponential(SmearPoints& smear_points, float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color);
}; // namespace smear
