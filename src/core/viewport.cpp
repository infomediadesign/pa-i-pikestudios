#include "include/pscore/viewport.h"

#include <cmath>

using PSCore::Viewport;

Vector2 Viewport::viewport_base_size()
{
	return m_viewport_base_size;
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
	auto screen_widthf	= static_cast<float>(GetScreenWidth());
	auto screen_heightf = static_cast<float>(GetScreenHeight());

	m_viewport_scale  = static_cast<float>(std::min(truncf(screen_widthf / m_viewport_base_size.x), truncf(screen_heightf / m_viewport_base_size.y)));
	m_viewport_origin = {
			(screen_widthf - m_viewport_base_size.x * m_viewport_scale) / 2, (screen_heightf - m_viewport_base_size.y * m_viewport_scale) / 2
	};
	m_viewport_base_origin = {(screen_widthf - m_viewport_base_size.x) / 2, (screen_heightf - m_viewport_base_size.y) / 2};
}

void Viewport::draw_outline_boxes(const Color& color)
{
	int screen_width  = GetScreenWidth();
	int screen_height = GetScreenHeight();

	auto screen_widthf	= static_cast<float>(screen_width);
	auto screen_heightf = static_cast<float>(screen_height);

	int screen_width_offset	 = static_cast<int>(screen_widthf - m_viewport_base_size.x * m_viewport_scale);
	int screen_height_offset = static_cast<int>(screen_heightf - m_viewport_base_size.y * m_viewport_scale);

	if ( m_viewport_base_size.x * m_viewport_scale < screen_widthf ) {
		DrawRectangle(0, 0, screen_width_offset / 2, screen_height, color);
		DrawRectangle(screen_width - (screen_width_offset / 2), 0, screen_width_offset / 2, screen_height, color);
	}
	if ( m_viewport_base_size.y * m_viewport_scale < screen_heightf ) {
		DrawRectangle(0, 0, screen_width, screen_height_offset / 2, color);
		DrawRectangle(0, screen_height - (screen_height_offset / 2), screen_width, screen_height_offset / 2, color);
	}
}

void Viewport::draw_viewport_frame(
		const bool& draw, const float& line_thickness, const float& radius, const Color& color_base, const Color& color_scaled
)
{
	if ( draw ) {
		DrawRectangleLinesEx(
				{m_viewport_base_origin.x, m_viewport_base_origin.y, m_viewport_base_size.x, m_viewport_base_size.y}, line_thickness, color_base
		);
		DrawCircleV(m_viewport_base_origin, radius, color_base);
		DrawRectangleLinesEx(
				{m_viewport_origin.x, m_viewport_origin.y, m_viewport_base_size.x * m_viewport_scale, m_viewport_base_size.y * m_viewport_scale},
				line_thickness, color_scaled
		);
		DrawCircleV(m_viewport_origin, radius, color_scaled);
	}
}


void Viewport::render()
{
	draw_outline_boxes(BLUE);

	draw_viewport_frame(false, 2, 10, RED, GREEN);
}
