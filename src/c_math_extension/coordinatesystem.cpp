#include <cmath>
#include <coordinatesystem.h>

#include "raymath.h"

//Functions are based on an Image-Coordinate-system if you use it in a Normal Coordinate-system have in mind that you must swap the left and right Functions

Vector2 forward_vector(float rotation)
{
	Vector2 Vector;

	Vector.x = cosf(rotation * DEG2RAD);
	Vector.y = sinf(rotation * DEG2RAD);

	return Vector;
}

Vector2 backward_vector(float rotation)
{
	Vector2 Vector;

	Vector.x = -cosf(rotation * DEG2RAD);
	Vector.y = -sinf(rotation * DEG2RAD);

	return Vector;
}

Vector2 right_vector(float rotation)
{
	Vector2 Vector;

	Vector.x = -sinf(rotation * DEG2RAD);
	Vector.y = cosf(rotation * DEG2RAD);

	return Vector;
}

Vector2 left_vector(float rotation)
{
	Vector2 Vector;

	Vector.x = sinf(rotation * DEG2RAD);
	Vector.y = -cosf(rotation * DEG2RAD);

	return Vector;
}

Vector2 relative_to_global_point_rightup(Vector2 origin, float rotation, Vector2 point)
{
	return origin + Vector2Scale(forward_vector(rotation), point.x) + Vector2Scale(left_vector(rotation), point.y);
}

Vector2 relative_to_global_point_rightdown(Vector2 origin, float rotation, Vector2 point)
{
	return origin + Vector2Scale(forward_vector(rotation), point.x) + Vector2Scale(right_vector(rotation), point.y);
}

Vector2 relative_to_global_point_leftup(Vector2 origin, float rotation, Vector2 point)
{
	return origin + Vector2Scale(backward_vector(rotation), point.x) + Vector2Scale(left_vector(rotation), point.y);
}

Vector2 relative_to_global_point_leftdown(Vector2 origin, float rotation, Vector2 point)
{
	return origin + Vector2Scale(backward_vector(rotation), point.x) + Vector2Scale(right_vector(rotation), point.y);
}

Vector2 global_to_relative_point_rightup(Vector2 origin, float rotation, Vector2 point)
{
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.y *= -1;

	return Vector;
}

Vector2 global_to_relative_point_rightdown(Vector2 origin, float rotation, Vector2 point)
{
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	return Vector;
}

Vector2 global_to_relative_point_leftup(Vector2 origin, float rotation, Vector2 point)
{
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.x *= -1;
	Vector.y *= -1;

	return Vector;
}

Vector2 global_to_relative_point_leftdown(Vector2 origin, float rotation, Vector2 point)
{
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.x *= -1;

	return Vector;
}
