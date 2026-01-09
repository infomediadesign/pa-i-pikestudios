#pragma once

#include <raylib.h>

namespace PSUtils {

	// General

	/*!
	 * @brief Generates a random number between min and max
	 * @param min: the minimum value
	 * @param max: the maximum value
	 */
	int gen_rand(const int min, const int max);



	// Raylib specific
	void DrawTextBoxedSelectable(
			Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength,
			Color selectTint, Color selectBackTint
	);

	void DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);

} // namespace PSUtils
