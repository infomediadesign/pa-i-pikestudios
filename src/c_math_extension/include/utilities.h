#pragma once

#include <raylib.h>

namespace utilities {

	/**
	 * @brief Calculates dividend divided by the divisor considering divided by 0
	 * @param dividend The Value above the Fractional line
	 * @param divisor The Value below the Fractional line
	 * @return The dividend divided by the divisor
	 * or if the divisor is 0 the return value is 0
	 */
	float divide_save(float dividend, float divisor);

	/**
	 * @brief Calculates dividend divided by the divisor considering divided by 0
	 * @param dividend The Value above the Fractional line
	 * @param divisor The Value below the Fractional line
	 * @param value The Value the returns if the divisor is 0
	 * @return The dividend divided by the divisor
	 * or if the divisor is 0 the return value is value
	 */
	float divide_save(float dividend, float divisor, float value);

	/**
	 * @brief Calculates the Position in a horizontal Grid based on the Index
	 * {0,1}
	 * {2,3}
	 * @param index The Index of the Grid Cell
	 * @param grid_width The Width of the Grid
	 * @param grid_height The Height of the Grid
	 * @return The Position in the Grid based on the Index
	 * or if the Index is not in range of the Grid the return value is {-1,-1}
	 */
	Vector2 grid_from_index_horizontal(int index, int grid_width, int grid_height);

	/**
	 * @brief Calculates the Position in a vertical Grid based on the Index
	 * {0,2}
	 * {1,3}
	 * @param index The Index of the Grid Cell
	 * @param grid_width The Width of the Grid
	 * @param grid_height The Height of the Grid
	 * @return The Position in the Grid based on the Index
	 * or if the Index is not in range of the Grid the return value is {-1,-1}
	 */
	Vector2 grid_from_index_vertical(int index, int grid_width, int grid_height);

	/**
	 * @brief Calculates a Vector with a certain length based on a Vector and a length
	 * @param vector A direction Vector
	 * @param length The length the direction Vector should have
	 * @return A Vector in the same direction as the direction Vector but with the given length
	 */
	Vector2 vector_with_length(Vector2 vector, float length);

	/**
	 * @brief Calculates a Rotation that the self Actor must be Rotated to look at a Target
	 * based on the self Location and the Target Location
	 * @param self_position Position of the self Actor
	 * @param look_position Position of the Point the self Actor is looking
	 * @return A Rotation that the self Actor must be Rotated to look at the Target
	 */
	float rotation_look_at(Vector2 self_position, Vector2 look_position);

} // namespace utilities
