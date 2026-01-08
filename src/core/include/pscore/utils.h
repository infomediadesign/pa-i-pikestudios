#pragma once

#include <raylib.h>
#include <type_traits>

namespace PSUtils {

	// General
	int gen_rand(const int min, const int max);

	

	// Raylib specific
	void DrawTextBoxedSelectable(
			Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength,
			Color selectTint, Color selectBackTint
	);

	void DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);

} // namespace PSUtils
