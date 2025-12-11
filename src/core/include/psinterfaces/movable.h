#pragma once

#include <raylib.h>
namespace PSInterfaces {

	/*! @brief Interface class for movable entities
	 *
	 */
	class IMovable
	{
	public:
		~IMovable() = default;

		/*!
		 *
		 */
		Vector2 pos();

		/*!
		 *
		 */
		Vector2 vel();

		/*!
		 *
		 */
		float rot();

		/*!
		 *
		 */
		void set_pos(const Vector2& pos);

		/*!
		 *
		 */
		void set_vel(const Vector2& vel);

		/*!
		 *
		 */
		void set_rot(const float rot);

		/*!
		 *
		 */
		virtual void move(const float dt);

	protected:
		Vector2 m_pos;
		Vector2 m_vel;
		float m_rot;
	};
} // namespace PSInterfaces
