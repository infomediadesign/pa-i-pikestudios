#pragma once

#include <memory>
#include <vector>
#include "raylib.h"

class Player;
class AppLayer;

/*!
 * @details Namespace for miscellaneous funktions
 */
namespace misc {

	/*!
	 * @details Namespace for map interactions
	 */
	namespace map {

		/**
		 * @brief Enum to determine the last collision axis.
		 */
		enum class LastCollisionAxis { None, Horizontal, Vertical };

		/**
		 * @brief Structure for a spawn request of a new player.
		 */
		struct SpawnRequest
		{
			Vector2 position; ///< Position of the new player
			Vector2 velocity; ///< Velocity of the new player
			float rotation; ///< Rotation of the new player
			float height; ///< Height of the player
			float width; ///< Width of the player
			LastCollisionAxis axis; ///< Axis of the last collision
		};

		/**
		 * @brief Enum for the wrap-around mode.
		 */
		enum WrapAroundMode { OnScreen, OffScreen };

		/**
		 * @brief Main function for map border wrap-around.
		 * @param player Reference to the player to be checked.
		 * @note This function should be called in the AppLayer update loop.
		 */
		void map_border_wrap_around(Player& player);

		/**
		 * @brief Spawns a new player at the opposite border.
		 * @param request The spawn request with the required parameters.
		 * @param appLayer Pointer to the AppLayer that manages the player.
		 * @note Only used when WrapAroundMode is set to OnScreen.
		 */
		void spawn_new_player_at_opposite_border(const SpawnRequest& request, AppLayer* appLayer);

		/**
		 * @brief Sets the player position to the opposite border.
		 * @param p Reference to the player.
		 * @param axis The axis on which the collision occurred.
		 * @note Only used when WrapAroundMode is set to OffScreen.
		 */
		void set_player_position_to_opposite_border(Player& p, LastCollisionAxis axis);

		/**
		 * @brief Checks if the player is completely off-screen.
		 * @param p Reference to the player.
		 * @return true if the player is completely off-screen, false otherwise.
		 */
		bool is_off_screen(Player& p);

		/**
		 * @brief Detects collisions with the map border and handles wrap-around.
		 * @param p Reference to the player.
		 * @param spawnRequests Vector of spawn requests that are filled on collision.
		 */
		void detect_map_border_collision(Player& p, std::vector<SpawnRequest>& spawnRequests);

		/**
		 * @brief Checks for horizontal collision (left/right).
		 * @param p Reference to the player.
		 * @param player_pos Current position of the player.
		 * @param screenW Width of the screen.
		 * @return true on collision with left or right border, false otherwise.
		 */
		bool check_collision_horizontal(Player& p, Vector2 player_pos, int screenW);

		/**
		 * @brief Checks for vertical collision (top/bottom).
		 * @param p Reference to the player.
		 * @param player_pos Current position of the player.
		 * @param screenH Height of the screen.
		 * @return true on collision with top or bottom border, false otherwise.
		 */
		bool check_collision_vertical(Player& p, Vector2 player_pos, int screenH);

		/**
		 * @brief Creates a spawn request and adds it to the vector.
		 * @param p Reference to the player.
		 * @param spawnRequests Vector to which the new request is added.
		 * @param axis Collision axis for the spawn position.
		 */
		void request_spawn(Player& p, std::vector<SpawnRequest>& spawnRequests, LastCollisionAxis axis);

		/**
		 * @brief Calculates the opposite position based on the axis.
		 * @param pos Current position.
		 * @param half_w Half width of the player.
		 * @param half_h Half height of the player.
		 * @param axis The axis for calculation.
		 * @return The calculated opposite position.
		 */
		Vector2 calculate_opposite_position(Vector2 pos, float half_w, float half_h, LastCollisionAxis axis);

		/**
		 * @brief Handles the on-screen wrap-around logic.
		 * @param p Reference to the player.
		 * @param player_pos Reference to the player position.
		 * @param screenW Width of the screen.
		 * @param screenH Height of the screen.
		 * @param spawnRequests Vector for spawn requests.
		 */
		void use_on_screen_wrap_around(Player& p, Vector2& player_pos, int screen_w, int screen_h, std::vector<SpawnRequest>& spawnRequests);

		/**
		 * @brief Handles the off-screen wrap-around logic.
		 * @param p Reference to the player.
		 * @param player_pos Reference to the player position.
		 * @param screenW Width of the screen.
		 * @param screenH Height of the screen.
		 */
		void use_off_screen_wrap_around(Player& p, Vector2& player_pos, int screen_w, int screen_h);

		/**
		 * @brief Processes entities that are off-screen.
		 * @note Destroys entities when WrapAroundMode is set to OffScreen.
		 */
		void process_off_screen_entities();

		/**
		 * @brief Toggles between on-screen and off-screen wrap-around modes.
		 */
		void toggle_wrap_around_mode();

	} // namespace map
} // namespace misc
