#pragma once

#include <raylib.h>

Vector2 vector_forward(float rotation);

Vector2 vector_backward(float rotation);

Vector2 vector_right(float rotation);

Vector2 vector_left(float rotation);

Vector2 point_relative_to_global_rightup(Vector2 origin, float rotation, Vector2 point);

Vector2 point_relative_to_global_rightdown(Vector2 origin, float rotation, Vector2 point);

Vector2 point_relative_to_global_leftup(Vector2 origin, float rotation, Vector2 point);

Vector2 point_relative_to_global_leftdown(Vector2 origin, float rotation, Vector2 point);

Vector2 point_global_to_relative_rightup(Vector2 origin, float rotation, Vector2 point);

Vector2 point_global_to_relative_rightdown(Vector2 origin, float rotation, Vector2 point);

Vector2 point_global_to_relative_leftup(Vector2 origin, float rotation, Vector2 point);

Vector2 point_global_to_relative_leftdown(Vector2 origin, float rotation, Vector2 point);