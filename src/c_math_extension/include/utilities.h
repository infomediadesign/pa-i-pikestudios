#pragma once

#include <raylib.h>

float divide_save(float dividend, float divisor);

float divide_save_pro(float dividend, float divisor, float value);

Vector2 grid_from_index_horizontal(int index, int grid_width, int grid_height);

Vector2 grid_from_index_vertical(int index, int grid_width, int grid_height);
