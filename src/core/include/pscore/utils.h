#pragma once

#include <raylib.h>
#include <type_traits>

namespace PSUtils {

	// General
	int gen_rand(const int min, const int max);

	// Debug
	template<typename VAL>
		requires(std::is_arithmetic_v<VAL>)
	void d_expose(const VAL* val)
	{
		
	}

	// Raylib specific
	void DrawTextBoxedSelectable(
			Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength,
			Color selectTint, Color selectBackTint
	);

	void DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
	
	template <typename T>
	T clamp_with_overflow(T min, T max, T val) {
		if ( val > max ) {
			T overflow  = val - max;
			return min + overflow;
		} else if ( val < min ) {
			T underflow = min - val;
			return max - underflow;
		}
		
		return val;
	};

} // namespace PSUtils
