#include "coordinatesystem.h"
#include <cmath>
#include <raymath.h>

Vector2 coordinatesystem::vector_forward(float rotation)
{
	Vector2 Vector;

	Vector.x = cosf(rotation * DEG2RAD);
	Vector.y = sinf(rotation * DEG2RAD);

	return Vector;
}

Vector2 coordinatesystem::vector_backward(float rotation)
{
	Vector2 Vector;

	Vector.x = -cosf(rotation * DEG2RAD);
	Vector.y = -sinf(rotation * DEG2RAD);

	return Vector;
}

Vector2 coordinatesystem::vector_right(float rotation)
{
	Vector2 Vector;

	Vector.x = sinf(rotation * DEG2RAD);
	Vector.y = -cosf(rotation * DEG2RAD);

	return Vector;
}

Vector2 coordinatesystem::vector_left(float rotation)
{
	Vector2 Vector;

	Vector.x = -sinf(rotation * DEG2RAD);
	Vector.y = cosf(rotation * DEG2RAD);

	return Vector;
}

Vector2 coordinatesystem::point_relative_to_global_rightup(Vector2 origin, float rotation, Vector2 point)
{
	return origin + Vector2Scale(vector_forward(rotation), point.x) + Vector2Scale(vector_left(rotation), point.y);
}

Vector2 coordinatesystem::point_relative_to_global_rightdown(Vector2 origin, float rotation, Vector2 point)
{
	return origin + Vector2Scale(vector_forward(rotation), point.x) + Vector2Scale(vector_right(rotation), point.y);
}

Vector2 coordinatesystem::point_relative_to_global_leftup(Vector2 origin, float rotation, Vector2 point)
{
	return origin + Vector2Scale(vector_backward(rotation), point.x) + Vector2Scale(vector_left(rotation), point.y);
}

Vector2 coordinatesystem::point_relative_to_global_leftdown(Vector2 origin, float rotation, Vector2 point)
{
	return origin + Vector2Scale(vector_backward(rotation), point.x) + Vector2Scale(vector_right(rotation), point.y);
}

Vector2 coordinatesystem::point_global_to_relative_rightup(Vector2 origin, float rotation, Vector2 point)
{
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	return Vector;
}

Vector2 coordinatesystem::point_global_to_relative_rightdown(Vector2 origin, float rotation, Vector2 point)
{
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.y *= -1;

	return Vector;
}

Vector2 coordinatesystem::point_global_to_relative_leftup(Vector2 origin, float rotation, Vector2 point)
{
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.x *= -1;

	return Vector;
}

Vector2 coordinatesystem::point_global_to_relative_leftdown(Vector2 origin, float rotation, Vector2 point)
{
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.x *= -1;
	Vector.y *= -1;

	return Vector;
}

std::vector<Vector2> coordinatesystem::points_relative_to_globle_rightup(Vector2 origin, float rotation, std::vector<Vector2>& points)
{
	std::vector<Vector2> relative_points;

	for ( auto point: points ) {
		relative_points.push_back(point_relative_to_global_rightup(origin, rotation, point));
	}

	return relative_points;
}
