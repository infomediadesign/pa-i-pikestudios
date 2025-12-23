#pragma once

#include <raylib.h>

Vector2 forward_vector(float rotation);

Vector2 backward_vector(float rotation);

Vector2 right_vector(float rotation);

Vector2 left_vector(float rotation);

Vector2 relative_to_global_point_rightup(Vector2 origin, float rotation, Vector2 point);

Vector2 relative_to_global_point_rightdown(Vector2 origin, float rotation, Vector2 point);

Vector2 relative_to_global_point_leftup(Vector2 origin, float rotation, Vector2 point);

Vector2 relative_to_global_point_leftdown(Vector2 origin, float rotation, Vector2 point);

Vector2 global_to_relative_point_rightup(Vector2 origin, float rotation, Vector2 point);

Vector2 global_to_relative_point_rightdown(Vector2 origin, float rotation, Vector2 point);

Vector2 global_to_relative_point_leftup(Vector2 origin, float rotation, Vector2 point);

Vector2 global_to_relative_point_leftdown(Vector2 origin, float rotation, Vector2 point);