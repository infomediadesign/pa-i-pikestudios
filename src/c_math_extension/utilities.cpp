#include "utilities.h"

#include "raymath.h"

float utilities::divide_save(float dividend, float divisor)
{
	if ( divisor == 0 ) {
		return 0;
	}
	return dividend / divisor;
}

float utilities::divide_save(float dividend, float divisor, float value)
{
	if ( divisor == 0 ) {
		return value;
	}
	return dividend / divisor;
}

Vector2 utilities::grid_from_index_horizontal(int index, int grid_width, int grid_height)
{
	Vector2 value = {0};

	if ( index >= grid_width * grid_height ) {
		return {-1, -1};
	}

	value.y = index / grid_width;
	value.x = index % grid_width;

	return value;
}

Vector2 utilities::grid_from_index_vertical(int index, int grid_width, int grid_height)
{
	Vector2 value = {0};

	if ( index >= grid_width * grid_height ) {
		return {-1, -1};
	}

	value.y = index / grid_height;
	value.x = index % grid_height;

	return value;
}

Vector2 utilities::vector_with_length(Vector2 vector, float length)
{
	return Vector2Scale(Vector2Normalize(vector), length);
}

float utilities::rotation_look_at(Vector2 self_position, Vector2 look_position)
{
	return atan2f(look_position.y - self_position.y, look_position.x - self_position.x) * RAD2DEG;
}
