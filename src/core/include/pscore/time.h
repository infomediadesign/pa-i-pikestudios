#pragma once

#include <chrono>

namespace PSCore {

	/*!
	 * @detail A class for managing delta time
	 */
	class DeltaTimeManager
	{
	public:
		~DeltaTimeManager() = default;

		/*!
		 * @brief calculates the delta time
		 * @warning If a target_fps is set, this will also freeze the current thread o meet the target. Use this carefully
		 */
		void calc_delta_t();

		/*!
		 * @brief returns the last calculated delta time
		 * @return delta time as a chrono duration in seconds
		 */
		std::chrono::duration<double> delta_t();

		/*!
		 * @brief Sets the internal target fps
		 * @param fps: the target fps
		 */
		void set_target_fps(int fps);

		/*!
		 * @brief Returns the last set target fps
		 * @return Last target fps as int
		 */
		int target_fps();

	private:
		std::chrono::duration<double> m_delta_t;
		std::chrono::time_point<std::chrono::steady_clock> m_current_t = std::chrono::steady_clock::now();
		std::chrono::time_point<std::chrono::steady_clock> m_prev_t;
		std::chrono::duration<double> m_update_t, m_wait_t;

		int m_target_fps = 0;
	};
} // namespace PSCore
