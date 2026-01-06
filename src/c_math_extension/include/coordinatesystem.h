#pragma once

#include <raylib.h>

/**
 * @details Functions are based on a Normal Coordinate-system
 * if you use it in an Image-Coordinate-system have in mind that you must swap the left and right Destination Vectors and the Up and Down
 * Coordinate-system Functions
 */
namespace coordinatesystem {

	/**
	 * @brief Calculate the Forward Vector of the Object
	 * @param rotation The Rotation of the Object
	 * @return The Forward Vector of the Object
	 */
	Vector2 vector_forward(float rotation);

	/**
	 * @brief Calculate the Backward Vector of the Object
	 * @param rotation The Rotation of the Object
	 * @return The Backward Vector of the Object
	 */
	Vector2 vector_backward(float rotation);

	/**
	 * @brief Calculate the Rightward Vector of the Object
	 * @param rotation The Rotation of the Object
	 * @return The Rightward Vector of the Object
	 */
	Vector2 vector_right(float rotation);

	/**
	 * @brief Calculate the Leftward Vector of the Object
	 * @param rotation The Rotation of the Object
	 * @return The Leftward Vector of the Object
	 */
	Vector2 vector_left(float rotation);

	/**
	 * @brief Calculates a Point in a Relative Coordinate system with positive Axis in right and up direction
	 * to a Point in the Global Coordinate system
	 * @param origin The Origin of the Relative Coordinate system
	 * @param rotation The Rotation of the Relative Coordinate system
	 * @param point The Relative Point
	 * @return The Coordinate of the Point in the Global Coordinate system
	 */
	Vector2 point_relative_to_global_rightup(Vector2 origin, float rotation, Vector2 point);

	/**
	 * @brief Calculates a Point in a Relative Coordinate system with positive Axis in right and down direction
	 * to a Point in the Global Coordinate system
	 * @param origin The Origin of the Relative Coordinate system
	 * @param rotation The Rotation of the Relative Coordinate system
	 * @param point The Relative Point
	 * @return The Coordinate of the Point in the Global Coordinate system
	 */
	Vector2 point_relative_to_global_rightdown(Vector2 origin, float rotation, Vector2 point);

	/**
	 * @brief Calculates a Point in a Relative Coordinate system with positive Axis in left and up direction
	 * to a Point in the Global Coordinate system
	 * @param origin The Origin of the Relative Coordinate system
	 * @param rotation The Rotation of the Relative Coordinate system
	 * @param point The Relative Point
	 * @return The Coordinate of the Point in the Global Coordinate system
	 */
	Vector2 point_relative_to_global_leftup(Vector2 origin, float rotation, Vector2 point);

	/**
	 * @brief Calculates a Point in a Relative Coordinate system with positive Axis in left and down direction
	 * to a Point in the Global Coordinate system
	 * @param origin The Origin of the Relative Coordinate system
	 * @param rotation The Rotation of the Relative Coordinate system
	 * @param point The Relative Point
	 * @return The Coordinate of the Point in the Global Coordinate system
	 */
	Vector2 point_relative_to_global_leftdown(Vector2 origin, float rotation, Vector2 point);

	/**
	 * @brief Calculates a Global Point to a Point in a Relative Coordinate system
	 * with positive Axis in right and up direction
	 * @param origin The Origin of the Relative Coordinate system
	 * @param rotation The Rotation of the Relative Coordinate system
	 * @param point The Global Point
	 * @return The Relative Version of the Global Point
	 */
	Vector2 point_global_to_relative_rightup(Vector2 origin, float rotation, Vector2 point);

	/**
	 * @brief Calculates a Global Point to a Point in a Relative Coordinate system
	 * with positive Axis in right and down direction
	 * @param origin The Origin of the Relative Coordinate system
	 * @param rotation The Rotation of the Relative Coordinate system
	 * @param point The Global Point
	 * @return The Relative Version of the Global Point
	 */
	Vector2 point_global_to_relative_rightdown(Vector2 origin, float rotation, Vector2 point);

	/**
	 * @brief Calculates a Global Point to a Point in a Relative Coordinate system
	 * with positive Axis in left and up direction
	 * @param origin The Origin of the Relative Coordinate system
	 * @param rotation The Rotation of the Relative Coordinate system
	 * @param point The Global Point
	 * @return The Relative Version of the Global Point
	 */
	Vector2 point_global_to_relative_leftup(Vector2 origin, float rotation, Vector2 point);

	/**
	 * @brief Calculates a Global Point to a Point in a Relative Coordinate system
	 * with positive Axis in left and down direction
	 * @param origin The Origin of the Relative Coordinate system
	 * @param rotation The Rotation of the Relative Coordinate system
	 * @param point The Global Point
	 * @return The Relative Version of the Global Point
	 */
	Vector2 point_global_to_relative_leftdown(Vector2 origin, float rotation, Vector2 point);

} // namespace coordinatesystem
