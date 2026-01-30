#include "smear.h"

#include <algorithm>
#include <coordinatesystem.h>
#include <raymath.h>

#include "utilities.h"

void Smear::update_smear(float actor_rotation_velocity, float deflection_scale, float deflection_velocity, float delta_time)
{
	m_smear_rotation += (deflection_scale * (actor_rotation_velocity) -m_smear_rotation) * std::clamp(deflection_velocity * delta_time, 0.0f, 1.0f);
}

void Smear::calculate_linear_smear(
		Vector2& start_position, float actor_velocity, float actor_rotation, float smear_length, float smear_rotation_offset,
		int smear_index
)
{
	while ( m_smear_points.size() - 1 < smear_index ) {
		m_smear_points.push_back({0});
	}

	Vector2 bv = coordinatesystem::vector_backward(actor_rotation);

	Vector2 p1 = start_position;
	Vector2 c2 = start_position + Vector2Rotate(Vector2Scale(bv, smear_length * actor_velocity / 2), smear_rotation_offset * DEG2RAD);
	Vector2 p3 = start_position + Vector2Rotate(Vector2Scale(bv, smear_length * actor_velocity), (m_smear_rotation + smear_rotation_offset) * DEG2RAD);

	m_smear_points.at(smear_index) = {p1, c2, {0}, p3};
}

void Smear::calculate_exponential_smear(
		Vector2& start_position, float actor_velocity, float actor_rotation, float smear_length, float smear_rotation_offset,
		float smear_deflection_start, float smear_deflection_length, int smear_index
)
{
	while ( m_smear_points.size() - 1 < smear_index ) {
		m_smear_points.push_back({0});
	}

	Vector2 bv = coordinatesystem::vector_backward(actor_rotation);
	Vector2 rv = coordinatesystem::vector_right(actor_rotation);

	Vector2 p1 = start_position;
	Vector2 c2 = start_position + Vector2Rotate(Vector2Scale(rv, smear_deflection_start * actor_velocity), smear_rotation_offset * DEG2RAD);
	Vector2 c3 = start_position +
				 Vector2Rotate(
						 Vector2Add(Vector2Scale(bv, smear_length * actor_velocity / 2), Vector2Scale(rv, smear_deflection_length * actor_velocity)),
						 smear_rotation_offset * DEG2RAD
				 );
	Vector2 p4 = start_position +
				 Vector2Rotate(
						 Vector2Add(Vector2Scale(bv, smear_length * actor_velocity), Vector2Scale(rv, smear_deflection_length * actor_velocity)),
						 (m_smear_rotation + smear_rotation_offset) * DEG2RAD
				 );

	m_smear_points.at(smear_index) = {p1, c2, c3, p4};
}

void Smear::draw_smear(int smear_index, SmearType smear_type, float smear_line_thickness, float smear_line_thickness_falloff, Color smear_color)
{
	Vector2 p1;
	Vector2 c2;
	Vector2 c3;
	Vector2 p4;

	const float step = 1.0f / SPLINE_SEGMENT_DIVISIONS;

	Vector2 previous = m_smear_points.at(smear_index).p1;
	Vector2 current	 = {0};
	float t			 = 0.0f;

	Vector2 points[2 * SPLINE_SEGMENT_DIVISIONS + 2] = {0};

	switch ( smear_type ) {
		case Linear:
			// Modified DrawSplineSegmentBezierQuadratic() with LineThickness Falloff

			p1 = m_smear_points.at(smear_index).p1;
			c2 = m_smear_points.at(smear_index).c2;
			p4 = m_smear_points.at(smear_index).p4;

			for ( int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++ ) {
				t = step * (float) i;

				float a = powf(1.0f - t, 2);
				float b = 2.0f * (1.0f - t) * t;
				float c = powf(t, 2);

				// NOTE: The easing functions aren't suitable here because they don't take a control point
				current.y = a * p1.y + b * c2.y + c * p4.y;
				current.x = a * p1.x + b * c2.x + c * p4.x;

				float dy   = current.y - previous.y;
				float dx   = current.x - previous.x;
				float size = 0.5f * smear_line_thickness / sqrtf(dx * dx + dy * dy);

				float line_thickness_falloff =
						(((-std::clamp(smear_line_thickness_falloff, 0.0f, 1.0f) / (SPLINE_SEGMENT_DIVISIONS - 1)) * static_cast<float>(i - 1)) + 1);

				if ( i == 1 ) {
					points[0].x = previous.x + dy * size * line_thickness_falloff;
					points[0].y = previous.y - dx * size * line_thickness_falloff;
					points[1].x = previous.x - dy * size * line_thickness_falloff;
					points[1].y = previous.y + dx * size * line_thickness_falloff;
				}

				points[2 * i + 1].x = current.x - dy * size * line_thickness_falloff;
				points[2 * i + 1].y = current.y + dx * size * line_thickness_falloff;
				points[2 * i].x		= current.x + dy * size * line_thickness_falloff;
				points[2 * i].y		= current.y - dx * size * line_thickness_falloff;

				previous = current;
			}

			DrawTriangleStrip(points, 2 * SPLINE_SEGMENT_DIVISIONS + 2, smear_color);
			break;
		case Exponential:
			// Modified DrawSplineSegmentBezierCubic() with LineThickness Falloff

			p1 = m_smear_points.at(smear_index).p1;
			c2 = m_smear_points.at(smear_index).c2;
			c3 = m_smear_points.at(smear_index).c3;
			p4 = m_smear_points.at(smear_index).p4;

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

				float line_thickness_falloff =
						(((-std::clamp(smear_line_thickness_falloff, 0.0f, 1.0f) / (SPLINE_SEGMENT_DIVISIONS - 1)) * static_cast<float>(i - 1)) + 1);

				if ( i == 1 ) {
					points[0].x = previous.x + dy * size * line_thickness_falloff;
					points[0].y = previous.y - dx * size * line_thickness_falloff;
					points[1].x = previous.x - dy * size * line_thickness_falloff;
					points[1].y = previous.y + dx * size * line_thickness_falloff;
				}

				points[2 * i + 1].x = current.x - dy * size * line_thickness_falloff;
				points[2 * i + 1].y = current.y + dx * size * line_thickness_falloff;
				points[2 * i].x		= current.x + dy * size * line_thickness_falloff;
				points[2 * i].y		= current.y - dx * size * line_thickness_falloff;

				previous = current;
			}

			DrawTriangleStrip(points, 2 * SPLINE_SEGMENT_DIVISIONS + 2, smear_color);
			break;
	}
}

void Smear::update_smear_wave(
		Vector2 smear_indices, SmearType smear_type, float smear_wave_velocity, float smear_wave_height, float actor_velocity,
		float actor_max_velocity, float delta_time
)
{
	while ( m_smear_wave_points.size() < WAVE_VECTOR_SIZE_MIN ) {
		m_smear_wave_points.push_back({0});
	}

	for ( auto& value: m_smear_wave ) {
		if ( value > 0 ) {
			value += smear_wave_velocity * (actor_velocity / actor_max_velocity) * delta_time;
		}
		if ( value * actor_max_velocity / actor_velocity > 1 ) {
			value = 0;
		}
	}

	for ( int i = 0; auto& value: m_smear_wave ) {
		if ( value > 0 ) {
			switch ( smear_type ) {
				case Linear:
					m_smear_wave_points.at(i).p1 = GetSplinePointBezierQuad(
							m_smear_points.at(smear_indices.x).p1, m_smear_points.at(smear_indices.x).c2, m_smear_points.at(smear_indices.x).p4,
							m_smear_wave.at(i) / (actor_velocity / actor_max_velocity)
					);
					m_smear_wave_points.at(i).p4 = GetSplinePointBezierQuad(
							m_smear_points.at(smear_indices.y).p1, m_smear_points.at(smear_indices.y).c2, m_smear_points.at(smear_indices.y).p4,
							m_smear_wave.at(i) / (actor_velocity / actor_max_velocity)
					);
					m_smear_wave_points.at(i).c2 =
							m_smear_wave_points.at(i).p1 + Vector2Scale(m_smear_wave_points.at(i).p4 - m_smear_wave_points.at(i).p1, 0.5) +
							utilities::vector_with_length(
									Vector2Rotate(m_smear_wave_points.at(i).p4 - m_smear_wave_points.at(i).p1, (-90 - m_smear_rotation) * DEG2RAD),
									smear_wave_height
							);
					break;
				case Exponential:
					m_smear_wave_points.at(i).p1 = GetSplinePointBezierCubic(
							m_smear_points.at(smear_indices.x).p1, m_smear_points.at(smear_indices.x).c2, m_smear_points.at(smear_indices.x).c3,
							m_smear_points.at(smear_indices.x).p4, m_smear_wave.at(i) / (actor_velocity / actor_max_velocity)
					);
					m_smear_wave_points.at(i).p4 = GetSplinePointBezierCubic(
							m_smear_points.at(smear_indices.y).p1, m_smear_points.at(smear_indices.y).c2, m_smear_points.at(smear_indices.y).c3,
							m_smear_points.at(smear_indices.y).p4, m_smear_wave.at(i) / (actor_velocity / actor_max_velocity)
					);
					m_smear_wave_points.at(i).c2 =
							m_smear_wave_points.at(i).p1 + Vector2Scale(m_smear_wave_points.at(i).p4 - m_smear_wave_points.at(i).p1, 0.5) +
							utilities::vector_with_length(
									Vector2Rotate(m_smear_wave_points.at(i).p4 - m_smear_wave_points.at(i).p1, (-90 - m_smear_rotation) * DEG2RAD),
									smear_wave_height
							);
					break;
			}
		}

		i++;
	}
}

void Smear::add_smear_wave(
		float smear_wave_velocity, float smear_wave_frequency, float actor_velocity, float actor_max_velocity, float delta_time, int smear_timer_index
)
{
	while ( m_smear_wave.size() < WAVE_VECTOR_SIZE_MIN ) {
		m_smear_wave.push_back(0);
	}

	while ( m_smear_wave_timer.size() - 1 < smear_timer_index ) {
		m_smear_wave_timer.push_back(0);
	}

	m_smear_wave_timer.at(smear_timer_index) += delta_time;

	if ( m_smear_wave_timer.at(smear_timer_index) >= smear_wave_frequency / (actor_velocity / actor_max_velocity) ) {
		m_smear_wave_timer.at(smear_timer_index) = 0;

		if ( m_smear_wave.at(m_smear_wave_index) == 0 ) {
			m_smear_wave.at(m_smear_wave_index) += std::clamp(smear_wave_velocity * (actor_velocity / actor_max_velocity) * delta_time, 0.0f, 1.0f);
		}

		m_smear_wave_index++;

		if ( m_smear_wave_index == m_smear_wave.size() ) {
			m_smear_wave_index = 0;
		}
	}
}


void Smear::add_smear_wave_at_location(
		float smear_wave_start, float smear_wave_frequency, float actor_velocity, float actor_max_velocity, float delta_time, int smear_timer_index
)
{
	while ( m_smear_wave.size() < WAVE_VECTOR_SIZE_MIN ) {
		m_smear_wave.push_back(0);
	}

	while ( m_smear_wave_timer.size() - 1 < smear_timer_index ) {
		m_smear_wave_timer.push_back(0);
	}

	m_smear_wave_timer.at(smear_timer_index) += delta_time;

	if ( m_smear_wave_timer.at(smear_timer_index) >= smear_wave_frequency / (actor_velocity / actor_max_velocity) ) {
		m_smear_wave_timer.at(smear_timer_index) = 0;

		if ( m_smear_wave.at(m_smear_wave_index) == 0 ) {
			m_smear_wave.at(m_smear_wave_index) += std::clamp(smear_wave_start, 0.0f, 1.0f);
		}

		m_smear_wave_index++;

		if ( m_smear_wave_index == m_smear_wave.size() ) {
			m_smear_wave_index = 0;
		}
	}
}

void Smear::draw_smear_wave(
		float actor_velocity, float actor_max_velocity, float smear_wave_line_thickness, float smear_wave_line_thickness_falloff,
		Color smear_wave_color
)
{
	if ( m_smear_wave_points.size() != m_smear_wave.size() )
		return;

	for ( int i = 0; i < m_smear_wave_points.size(); i++ ) {
		if ( m_smear_wave.at(i) > 0 ) {

			Vector2 p1 = m_smear_wave_points.at(i).p1;
			Vector2 c2 = m_smear_wave_points.at(i).c2;
			Vector2 p3 = m_smear_wave_points.at(i).p4;

			const float step = 1.0f / SPLINE_SEGMENT_DIVISIONS;

			Vector2 previous = p1;
			Vector2 current	 = {0};
			float t			 = 0.0f;

			Vector2 points[2 * SPLINE_SEGMENT_DIVISIONS + 2] = {0};

			for ( int j = 1; j <= SPLINE_SEGMENT_DIVISIONS; j++ ) {
				t = step * (float) j;

				float a = powf(1.0f - t, 2);
				float b = 2.0f * (1.0f - t) * t;
				float c = powf(t, 2);

				// NOTE: The easing functions aren't suitable here because they don't take a control point
				current.y = a * p1.y + b * c2.y + c * p3.y;
				current.x = a * p1.x + b * c2.x + c * p3.x;

				float dy   = current.y - previous.y;
				float dx   = current.x - previous.x;
				float size = 0.5f * smear_wave_line_thickness / sqrtf(dx * dx + dy * dy);

				float line_thickness_falloff =
						fmaxf(1 - (smear_wave_line_thickness_falloff * m_smear_wave[i] / (actor_velocity / actor_max_velocity)), 0);

				if ( j == 1 ) {
					points[0].x = previous.x + dy * size * line_thickness_falloff;
					points[0].y = previous.y - dx * size * line_thickness_falloff;
					points[1].x = previous.x - dy * size * line_thickness_falloff;
					points[1].y = previous.y + dx * size * line_thickness_falloff;
				}

				points[2 * j + 1].x = current.x - dy * size * line_thickness_falloff;
				points[2 * j + 1].y = current.y + dx * size * line_thickness_falloff;
				points[2 * j].x		= current.x + dy * size * line_thickness_falloff;
				points[2 * j].y		= current.y - dx * size * line_thickness_falloff;

				previous = current;
			}

			DrawTriangleStrip(points, 2 * SPLINE_SEGMENT_DIVISIONS + 2, smear_wave_color);
		}
	}
}
