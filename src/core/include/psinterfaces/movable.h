#pragma once

#include <raylib.h>
namespace PSInterfaces {
	class IMovable
	{
	public:
		~IMovable() = default;

		Vector2 pos();
		Vector2 vel();
		int rot();

		void set_pos(const Vector2& pos);
		void set_vel(const Vector2& vel);
		void set_rot(const int rot);

		void move(const int dt);

	protected:
		Vector2 m_pos;
		Vector2 m_vel;
		int m_rot;
	};
} // namespace PSInterfaces
