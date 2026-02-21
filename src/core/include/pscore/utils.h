#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace PSUtils {

	// General

	/*!
	 * @brief Generates a random number between min and max
	 * @param min: the minimum value
	 * @param max: the maximum value
	 */
	int gen_rand(const int min, const int max);

	std::string generate_uid();

	template<typename T>
	class Grid2D
	{
	public:
		Grid2D(size_t width, size_t height) : data(width * height), w(width), h(height)
		{
		}

		T& operator()(size_t x, size_t y)
		{
			return data[y * w + x];
		}

		const T& operator()(size_t x, size_t y) const
		{
			return data[y * w + x];
		}

		int width() const
		{
			return w;
		}

		int height() const
		{
			return h;
		}

	private:
		std::vector<T> data;
		size_t w, h;
	};

	template<typename T>
	T clamp_with_overflow(T min, T max, T val)
	{
		if ( val > max ) {
			T overflow = val - max;
			return min + overflow;
		} else if ( val < min ) {
			T underflow = min - val;
			return max - underflow;
		}

		return val;
	};

	// Raylib specific
	void DrawTextBoxedSelectable(
			Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength,
			Color selectTint, Color selectBackTint
	);

	void DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);

	void DrawRectangleLinesRotated(Rectangle rec, float rotation, Color color);

} // namespace PSUtils
