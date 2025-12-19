#include "applayer.h"
#include "debuglayer.h"
#include "pauselayer.h"
#include "pscore/utils.h"

#include <array>
#include <deque>
#include <entt/entity/fwd.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <pscore/application.h>
#include <raylib.h>

#include <psinterfaces/movable.h>
#include <psinterfaces/renderable.h>

#include <entt/entt.hpp>
#include <thread>
#include <vector>

class AppLayerPriv
{
	friend class AppLayer;

	void handle_input()
	{
		auto app = PSCore::Application::get();
		if ( IsKeyPressed(KEY_F3) ) {
			if ( app->get_layer<DebugLayer>() )
				app->pop_layer<DebugLayer>();
			else
				app->push_layer<DebugLayer>();
		}

		if ( IsKeyPressed(KEY_ESCAPE) ) {
			if ( app->get_layer<PauseLayer>() ) {
				app->pop_layer<PauseLayer>();
				if ( auto app_layer = app->get_layer<AppLayer>() )
					app_layer->resume();
			} else {
				app->push_layer<PauseLayer>();
				if ( auto app_layer = app->get_layer<AppLayer>() )
					app_layer->suspend();
			}
		}
	}


	std::deque<Vector2> spline_points;
};

AppLayer::AppLayer()
{
	_p = std::make_unique<AppLayerPriv>();
}

AppLayer::~AppLayer()
{
}

void AppLayer::on_update(const float dt)
{
	_p->handle_input();

	if ( !active )
		return;
}

void AppLayer::on_render()
{
	int tile_size	  = 32;
	int screen_height = GetScreenHeight(), screen_width = GetScreenWidth();

	for ( int i = 0; i < screen_height; i++ )
		DrawLine(0, tile_size * i, screen_width, tile_size * i, GRAY);
	for ( int i = 0; i < screen_width / tile_size; i++ )
		DrawLine(tile_size * i, 0, tile_size * i, screen_height, GRAY);

	const auto gen_rand_anchor = [screen_width, screen_height, tile_size]() -> Vector2 {
		float x = PSUtils::gen_rand(3, (screen_width / tile_size) - 3);
		float y = PSUtils::gen_rand(3, (screen_height / tile_size) - 3);

		return {x * tile_size, y * tile_size};
	};

	enum MapEdge {
		Left = 0,
		Right,

		Top,
		Bottom
	};

	const auto gen_tip = [tile_size]() -> Vector2 {
		static MapEdge map_edge;

		MapEdge rand_map_edge = static_cast<MapEdge>(PSUtils::gen_rand(MapEdge::Left, MapEdge::Bottom));
		if ( rand_map_edge == map_edge )
			map_edge = static_cast<MapEdge>(PSUtils::clamp_with_overflow<int>(0, MapEdge::Bottom, map_edge + 1));
		else
			map_edge = rand_map_edge;

		float x, y;
		switch ( map_edge ) {
			case Left:
				x = 0;
				y = PSUtils::gen_rand(3, (GetScreenHeight() / tile_size) - 3);
				break;
			case Right:
				x = GetScreenWidth() / tile_size;
				y = PSUtils::gen_rand(3, (GetScreenHeight() / tile_size) - 3);
				break;
			case Top:
				x = PSUtils::gen_rand(3, (GetScreenHeight() / tile_size) - 3);
				y = 0;
				break;
			case Bottom:
				x = PSUtils::gen_rand(3, (GetScreenHeight() / tile_size) - 3);
				y = GetScreenHeight() / tile_size;
		}

		return {x * tile_size, y * tile_size};
	};

	auto& spline_points = _p->spline_points;
	if ( IsKeyPressed(KEY_S) ) {
		spline_points.clear();

		for ( int i = 0; i < 1; i++ )
			spline_points.push_back(gen_rand_anchor());

		const Vector2& first = gen_tip();
		spline_points.push_front(first);
		spline_points.push_front(first);

		const Vector2& last = gen_tip();
		spline_points.push_back(last);
		spline_points.push_back(last);
	}

	if ( spline_points.size() == 0 )
		return;

	for ( int i = 0; i < spline_points.size(); i++ ) {
		DrawRectangle(spline_points.at(i).x, spline_points.at(i).y, tile_size, tile_size, BLUE);
		DrawText(TextFormat("%i", i), spline_points.at(i).x + 3, spline_points.at(i).y + 3, 6, WHITE);
	}

	Vector2* arr = new Vector2[spline_points.size()];
	std::copy(spline_points.begin(), spline_points.end(), arr);

	DrawSplineCatmullRom(arr, spline_points.size(), 3, RED);
}
