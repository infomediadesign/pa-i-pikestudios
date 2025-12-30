#include "utilities.h"

float divide_save(float dividend, float divisor)
{
	// Calculate dividend / divisor or if divisor is 0 it returns 0
	if ( divisor == 0 ) {
		return 0;
	}
	return dividend / divisor;
}

float divide_save_pro(float dividend, float divisor, float value)
{
	// Calculate dividend / divisor or if divisor is 0 it returns the value
	if ( divisor == 0 ) {
		return value;
	}
	return dividend / divisor;
}

Vector2 grid_from_index_horizontal(int index, int grid_width, int grid_height)
{
	// Calculate the Index value to a position on a 2D Horizontal Grid or returns {-1,-1} if the Index is not in range of the Grid
	Vector2 value = {0};

	if ( index >= grid_width * grid_height ) {
		return {-1, -1};
	}

	value.y = index / grid_width;
	value.x = index % grid_width;

	return value;
}

Vector2 grid_from_index_vertical(int index, int grid_width, int grid_height)
{
	// Calculate the Index value to a position on a 2D Vertical Grid or returns {-1,-1} if the Index is not in range of the Grid
	Vector2 value = {0};

	if ( index >= grid_width * grid_height ) {
		return {-1, -1};
	}

	value.y = index / grid_height;
	value.x = index % grid_height;

	return value;
}
