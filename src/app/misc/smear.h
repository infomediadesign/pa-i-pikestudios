#pragma once

#include "raylib.h"

#ifndef SPLINE_SEGMENT_DIVISIONS
#define SPLINE_SEGMENT_DIVISIONS 24 // Spline segment divisions
#endif

namespace smear {
	typedef struct SmearLinearPoints
	{
		Vector2 p1;
		Vector2 c2;
		Vector2 p3;
	} SmearLinearPoints;

	typedef struct SmearExponentialPoints
	{
		Vector2 p1;
		Vector2 c2;
		Vector2 c3;
		Vector2 p4;
	} SmearExponentialPoints;

	float update_smear_rotation(
			float smear_rotation_current, float actor_rotation_velocity, float deflection_scale, float deflection_velocity, float delta_time
	);

	SmearLinearPoints draw_smear_linear(
			Vector2 position_start, float actor_velocity, float actor_rotation, float smear_rotation, float smear_length, float smear_rotation_offset,
			float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color
	);

	SmearExponentialPoints draw_smear_exponential(
			Vector2 position_start, float actor_velocity, float actor_rotation, float smear_rotation, float smear_length, float smear_rotation_offset,
			float smear_deflection_start, float smear_deflection_length, float smear_line_thickness, float smear_line_thickness_falloff,
			Color smear_color
	);
}; // namespace smear
