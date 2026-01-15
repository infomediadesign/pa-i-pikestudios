#pragma once

#include <raylib.h>
#include "psinterfaces/renderable.h"

namespace PSCore {

	class Viewport : public PSInterfaces::IRenderable
	{
	public:
		Viewport();

		void update(float dt) override;

		void render() override;

		Vector2 viewport_base_size();

		float viewport_scale();

		Vector2 viewport_base_origin();

		Vector2 viewport_origin();

		void set_viewport_base_size(const Vector2& viewport_base_size);

		/**
		 * @brief Draws a Texture in the defined Viewport with a Position in the Relative Coordinate system of the Viewport and Origin in the center
		 * of the Texture
		 * @param texture Texture to Draw
		 * @param source The source Rectangle of the Texture
		 * @param position The Position of the Texture in relation to the Viewport
		 * @param rotation The Rotation of the Texture
		 * @param color The Color overlay of the Texture (Default use WHITE)
		 */
		void draw_in_viewport(const Texture2D& texture, const Rectangle& source, const Vector2& position, float rotation, const Color& color);

		/**
		 * @brief Calculates a Position in the Viewport to a Global Position
		 * @param position_viewport A Position in the Viewport
		 * @return The Global Position
		 */
		Vector2 position_viewport_to_global(const Vector2& position_viewport);

	private:
		/**
		 * @brief Draws Outline Boxes between the Viewport border and the Window border
		 * @param color The color of the Outline Boxes
		 */
		void draw_outline_boxes(const Color& color);

		/**
		 * @brief Draws a Frame around the Base and Scaled Viewport and the Origin of the Base and Scaled Viewport
		 * @param draw If it Draws
		 * @param line_thickness The Line thickness of the Frames
		 * @param radius The Circle radius of the Origins
		 * @param color_base The Color of the Base Frame and Base Origin
		 * @param color_scaled The Color of the Scaled Frame and Scaled Origin
		 */
		void draw_viewport_frame(bool draw, float line_thickness, float radius, const Color& color_base, const Color& color_scaled);

		Vector2 m_viewport_base_size = {640, 360};

		float m_viewport_scale = 1;

		Vector2 m_viewport_base_origin = {(GetScreenWidth() - m_viewport_base_size.x) / 2, (GetScreenHeight() - m_viewport_base_size.y) / 2};

		Vector2 m_viewport_origin = {(GetScreenWidth() - m_viewport_base_size.x) / 2, (GetScreenHeight() - m_viewport_base_size.y) / 2};
	};

} // namespace PSCore
