#include <psinterfaces/movable.h>
#include <raylib.h>

Vector2 PSInterfaces::IMovable::pos()
{
	return m_pos;
}

Vector2 PSInterfaces::IMovable::vel()
{
	return m_vel;
}

int PSInterfaces::IMovable::rot()
{
	return m_rot;
}

void PSInterfaces::IMovable::set_pos(const Vector2& pos)
{
	m_pos = pos;
};

void PSInterfaces::IMovable::set_vel(const Vector2& vel)
{
	m_vel = vel;
};

void PSInterfaces::IMovable::set_rot(const int rot)
{
	m_rot = rot;
};

void PSInterfaces::IMovable::move(const float dt)
{
	m_pos.x += m_vel.x * dt;
	m_pos.y += m_vel.y * dt;
};
