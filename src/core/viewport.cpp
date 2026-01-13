#include "include/pscore/viewport.h"

#include <cmath>

Vector2 Viewport::viewport_base_size()
{
	return m_viewport_base_origin;
}

float Viewport::viewport_scale()
{
	return m_viewport_scale;
}

Vector2 Viewport::viewport_base_origin()
{
	return m_viewport_base_origin;
}

Vector2 Viewport::viewport_origin()
{
	return m_viewport_origin;
}

void Viewport::set_viewport_base_size(const Vector2& viewport_base_size)
{
	m_viewport_base_size = viewport_base_size;
}

void Viewport::draw_in_viewport(const Texture2D& texture, const Rectangle& source, const Vector2& position, const float& rotation, const Color& color)
{
	Rectangle dest = {
			m_viewport_origin.x + position.x * m_viewport_scale, m_viewport_origin.y + position.y * m_viewport_scale, source.width * m_viewport_scale,
			source.height * m_viewport_scale
	};
	Vector2 orign = {dest.width / 2, dest.height / 2};

	DrawTexturePro(texture, source, dest, orign, rotation, color);
}

Vector2 Viewport::position_viewport_to_global(const Vector2& position_viewport)
{
	return {m_viewport_origin.x + position_viewport.x * m_viewport_scale, m_viewport_origin.y + position_viewport.y * m_viewport_scale};
}

void Viewport::update(float dt)
{
	m_viewport_scale  = std::min(trunc(GetScreenWidth() / m_viewport_base_size.x), trunc(GetScreenHeight() / m_viewport_base_size.y));
	m_viewport_origin = {
			(GetScreenWidth() - m_viewport_base_size.x * m_viewport_scale) / 2, (GetScreenHeight() - m_viewport_base_size.y * m_viewport_scale) / 2
	};
	m_viewport_base_origin = {(GetScreenWidth() - m_viewport_base_size.x) / 2, (GetScreenHeight() - m_viewport_base_size.y) / 2};
}

void Viewport::draw_outline_boxes(const Color& color)
{
	if ( m_viewport_base_size.x * m_viewport_scale < GetScreenWidth() ) {
		DrawRectangle(0, 0, (GetScreenWidth() - m_viewport_base_size.x * m_viewport_scale) / 2, GetScreenHeight(), color);
		DrawRectangle(GetScreenWidth(), 0, -(GetScreenWidth() - m_viewport_base_size.x * m_viewport_scale) / 2, GetScreenHeight(), color);
	}
	if ( m_viewport_base_size.y * m_viewport_scale < GetScreenHeight() ) {
		DrawRectangle(0, 0, GetScreenWidth(), (GetScreenHeight() - m_viewport_base_size.y * m_viewport_scale) / 2, color);
		DrawRectangle(0, GetScreenHeight(), GetScreenWidth(), -(GetScreenHeight() - m_viewport_base_size.y * m_viewport_scale) / 2, color);
	}
}

void Viewport::render()
{
	draw_outline_boxes(BLACK);
}
