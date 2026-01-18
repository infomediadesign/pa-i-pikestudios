#include "smear.h"
#include <coordinatesystem.h>
#include <raymath.h>

float smear::update_smear_rotation(
		float smear_rotation_current, float actor_rotation_velocity, float deflection_scale, float deflection_velocity, float delta_time
)
{
	return smear_rotation_current + (deflection_scale * (actor_rotation_velocity) -smear_rotation_current) * (deflection_velocity * delta_time);
}

smear::SmearLinearPoints smear::draw_smear_linear(
		Vector2 position_start, float actor_velocity, float actor_rotation, float smear_rotation, float smear_length, float smear_rotation_offset,
		float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color
)
{
	Vector2 p1 = position_start;
	Vector2 c2 =
			position_start +
			Vector2Rotate(Vector2Scale(coordinatesystem::vector_backward(actor_rotation), smear_length * actor_velocity / 2), smear_rotation_offset);
	Vector2 p3 = position_start + Vector2Rotate(
										  Vector2Scale(coordinatesystem::vector_backward(actor_rotation), smear_length * actor_velocity),
										  smear_rotation + smear_rotation_offset
								  );

	const float step = 1.0f / SPLINE_SEGMENT_DIVISIONS;

	Vector2 previous = p1;
	Vector2 current	 = {0};
	float t			 = 0.0f;

	Vector2 points[2 * SPLINE_SEGMENT_DIVISIONS + 2] = {0};

	for ( int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++ ) {
		t = step * (float) i;

		float a = powf(1.0f - t, 2);
		float b = 2.0f * (1.0f - t) * t;
		float c = powf(t, 2);

		// NOTE: The easing functions aren't suitable here because they don't take a control point
		current.y = a * p1.y + b * c2.y + c * p3.y;
		current.x = a * p1.x + b * c2.x + c * p3.x;

		float dy   = current.y - previous.y;
		float dx   = current.x - previous.x;
		float size = 0.5f * smear_line_thickness / sqrtf(dx * dx + dy * dy);

		if ( i == 1 ) {
			points[0].x = previous.x + dy * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
			points[0].y = previous.y - dx * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
			points[1].x = previous.x - dy * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
			points[1].y = previous.y + dx * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
		}

		points[2 * i + 1].x = current.x - dy * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
		points[2 * i + 1].y = current.y + dx * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
		points[2 * i].x		= current.x + dy * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
		points[2 * i].y		= current.y - dx * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);

		previous = current;
	}

	DrawTriangleStrip(points, 2 * SPLINE_SEGMENT_DIVISIONS + 2, smear_color);

	return {p1, c2, p3};
}

smear::SmearExponentialPoints smear::draw_smear_exponential(
		Vector2 position_start, float actor_velocity, float actor_rotation, float smear_rotation, float smear_length, float smear_rotation_offset,
		float smear_deflection_start, float smear_deflection_length, float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color
)
{
	Vector2 p1 = position_start;
	Vector2 c2 = position_start +
				 Vector2Rotate(
						 Vector2Scale(coordinatesystem::vector_right(actor_rotation), smear_deflection_start * actor_velocity), smear_rotation_offset
				 );
	Vector2 c3 =
			position_start + Vector2Rotate(
									 Vector2Add(
											 Vector2Scale(coordinatesystem::vector_backward(actor_rotation), smear_length * actor_velocity / 2),
											 Vector2Scale(coordinatesystem::vector_right(actor_rotation), smear_deflection_length * actor_velocity)
									 ),
									 smear_rotation_offset
							 );
	Vector2 p4 =
			position_start + Vector2Rotate(
									 Vector2Add(
											 Vector2Scale(coordinatesystem::vector_backward(actor_rotation), smear_length * actor_velocity),
											 Vector2Scale(coordinatesystem::vector_right(actor_rotation), smear_deflection_length * actor_velocity)
									 ),
									 smear_rotation + smear_rotation_offset
							 );

	const float step = 1.0f / SPLINE_SEGMENT_DIVISIONS;

	Vector2 previous = p1;
	Vector2 current	 = {0};
	float t			 = 0.0f;

	Vector2 points[2 * SPLINE_SEGMENT_DIVISIONS + 2] = {0};

	for ( int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++ ) {
		t = step * (float) i;

		float a = powf(1.0f - t, 3);
		float b = 3.0f * powf(1.0f - t, 2) * t;
		float c = 3.0f * (1.0f - t) * powf(t, 2);
		float d = powf(t, 3);

		current.y = a * p1.y + b * c2.y + c * c3.y + d * p4.y;
		current.x = a * p1.x + b * c2.x + c * c3.x + d * p4.x;

		float dy   = current.y - previous.y;
		float dx   = current.x - previous.x;
		float size = 0.5f * smear_line_thickness / sqrtf(dx * dx + dy * dy);

		if ( i == 1 ) {
			points[0].x = previous.x + dy * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
			points[0].y = previous.y - dx * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
			points[1].x = previous.x - dy * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
			points[1].y = previous.y + dx * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
		}

		points[2 * i + 1].x = current.x - dy * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
		points[2 * i + 1].y = current.y + dx * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
		points[2 * i].x		= current.x + dy * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);
		points[2 * i].y		= current.y - dx * size * (((-smear_line_thickness_falloff / (SPLINE_SEGMENT_DIVISIONS - 1)) * (i - 1)) + 1);

		previous = current;
	}

	DrawTriangleStrip(points, 2 * SPLINE_SEGMENT_DIVISIONS + 2, smear_color);

	return {p1, c2, c3, p4};
}
