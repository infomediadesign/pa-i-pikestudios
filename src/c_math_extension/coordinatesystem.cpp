#include "coordinatesystem.h"
#include <raymath.h>
#include <cmath>

//Functions are based on a Normal Coordinate-system
//if you use it in an Image-Coordinate-system have in mind that you must swap the left and right Destination Vectors and the Up and Down Coordinate-system Functions

Vector2 vector_forward(float rotation)
{
	//Returns the Forward Vector of an Object based on its Rotation
	Vector2 Vector;

	Vector.x = cosf(rotation * DEG2RAD);
	Vector.y = sinf(rotation * DEG2RAD);

	return Vector;
}

Vector2 vector_backward(float rotation)
{
	//Returns the Backward Vector of an Object based on its Rotation
	Vector2 Vector;

	Vector.x = -cosf(rotation * DEG2RAD);
	Vector.y = -sinf(rotation * DEG2RAD);

	return Vector;
}

Vector2 vector_right(float rotation)
{
	//Returns the Right Vector of an Object based on its Rotation
	Vector2 Vector;

	Vector.x = sinf(rotation * DEG2RAD);
	Vector.y = -cosf(rotation * DEG2RAD);

	return Vector;
}

Vector2 vector_left(float rotation)
{
	//Returns the Left Vector of an Object based on its Rotation
	Vector2 Vector;

	Vector.x = -sinf(rotation * DEG2RAD);
	Vector.y = cosf(rotation * DEG2RAD);

	return Vector;
}

Vector2 point_relative_to_global_rightup(Vector2 origin, float rotation, Vector2 point)
{
	//Calculates a Point in a Relative right up Coordinate system to a Global Coordinate
	return origin + Vector2Scale(vector_forward(rotation), point.x) + Vector2Scale(vector_left(rotation), point.y);
}

Vector2 point_relative_to_global_rightdown(Vector2 origin, float rotation, Vector2 point)
{
	//Calculates a Point in a Relative right down Coordinate system to a Global Coordinate
	return origin + Vector2Scale(vector_forward(rotation), point.x) + Vector2Scale(vector_right(rotation), point.y);
}

Vector2 point_relative_to_global_leftup(Vector2 origin, float rotation, Vector2 point)
{
	//Calculates a Point in a Relative left up Coordinate system to a Global Coordinate
	return origin + Vector2Scale(vector_backward(rotation), point.x) + Vector2Scale(vector_left(rotation), point.y);
}

Vector2 point_relative_to_global_leftdown(Vector2 origin, float rotation, Vector2 point)
{
	//Calculates a Point in a Relative left down Coordinate system to a Global Coordinate
	return origin + Vector2Scale(vector_backward(rotation), point.x) + Vector2Scale(vector_right(rotation), point.y);
}

Vector2 point_global_to_relative_rightup(Vector2 origin, float rotation, Vector2 point)
{
	//Calculates a Global Point to a Coordinate in a Relative right up Coordinate system
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	return Vector;
}

Vector2 point_global_to_relative_rightdown(Vector2 origin, float rotation, Vector2 point)
{
	//Calculates a Global Point to a Coordinate in a Relative right down Coordinate system
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.y *= -1;

	return Vector;
}

Vector2 point_global_to_relative_leftup(Vector2 origin, float rotation, Vector2 point)
{
	//Calculates a Global Point to a Coordinate in a Relative left up Coordinate system
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.x *= -1;

	return Vector;
}

Vector2 point_global_to_relative_leftdown(Vector2 origin, float rotation, Vector2 point)
{
	//Calculates a Global Point to a Coordinate in a Relative left down Coordinate system
	Vector2 Vector;

	Vector = Vector2Rotate(point - origin, -rotation);

	Vector.x *= -1;
	Vector.y *= -1;

	return Vector;
}
