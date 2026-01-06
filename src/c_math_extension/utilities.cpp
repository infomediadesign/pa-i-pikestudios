#include "utilities.h"

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
