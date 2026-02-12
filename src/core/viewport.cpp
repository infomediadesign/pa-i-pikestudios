#include <pscore/viewport.h>

#include <imgui.h>
#include "psinterfaces/entity.h"

using PSCore::Viewport;

Viewport::Viewport() : PSInterfaces::IEntity("viewport")
{
	propose_z_index(100000);
}

void Viewport::update(float dt)
{
	auto screen_widthf	= static_cast<float>(GetScreenWidth());
	auto screen_heightf = static_cast<float>(GetScreenHeight());

	if ( m_sub_pixel_scale ) {
		m_viewport_scale = static_cast<float>(std::min(screen_widthf / m_viewport_base_size.x, screen_heightf / m_viewport_base_size.y));
	} else {
		m_viewport_scale =
				static_cast<float>(std::min(truncf(screen_widthf / m_viewport_base_size.x), truncf(screen_heightf / m_viewport_base_size.y)));
	}

	m_viewport_origin = {
			(screen_widthf - m_viewport_base_size.x * m_viewport_scale) / 2, (screen_heightf - m_viewport_base_size.y * m_viewport_scale) / 2
	};
	m_viewport_base_origin = {(screen_widthf - m_viewport_base_size.x) / 2, (screen_heightf - m_viewport_base_size.y) / 2};
}

void Viewport::render()
{
	draw_outline_boxes(BLACK);
}

void Viewport::draw_debug()
{
	draw_viewport_frame(m_draw_viewport_frame, 2, 10, RED, GREEN);

	ImGui::Checkbox("Render Viewport Frame", &m_draw_viewport_frame);
}


void Viewport::draw_in_viewport(const Texture2D& texture, const Rectangle& source, const Vector2& position, float rotation, const Color& color)
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

void Viewport::draw_viewport_frame(bool draw, float line_thickness, float radius, const Color& color_base, const Color& color_scaled)
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
