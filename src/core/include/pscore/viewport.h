#pragma once

#include <raylib.h>
#include "psinterfaces/renderable.h"

namespace PSCore {

	class Viewport : public PSInterfaces::IRenderable
	{
	public:
		Vector2 viewport_base_size();

		float viewport_scale();

		Vector2 viewport_base_origin();

		Vector2 viewport_origin();

		void set_viewport_base_size(const Vector2& viewport_base_size);

		void draw_in_viewport(const Texture2D& texture, const Rectangle& source, const Vector2& position, const float& rotation, const Color& color);

		Vector2 position_viewport_to_global(const Vector2& position_viewport);

		void update(float dt) override;

		void render() override;

	private:
		void draw_outline_boxes(const Color& color);

		Vector2 m_viewport_base_size = {640, 360};

		float m_viewport_scale = 1;

		Vector2 m_viewport_base_origin = {(GetScreenWidth() - m_viewport_base_size.x) / 2, (GetScreenHeight() - m_viewport_base_size.y) / 2};

		Vector2 m_viewport_origin = {(GetScreenWidth() - m_viewport_base_size.x) / 2, (GetScreenHeight() - m_viewport_base_size.y) / 2};
	};

} // namespace PSCore
