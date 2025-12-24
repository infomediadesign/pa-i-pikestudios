#pragma once

#include <raylib.h>

float save_divide(float dividend, float divisor);

float save_divide_pro(float dividend, float divisor, float value);

Vector2 index_to_horizontal_grid(int index, int grid_width, int grid_height);

Vector2 index_to_vertical_grid(int index, int grid_width, int grid_height);