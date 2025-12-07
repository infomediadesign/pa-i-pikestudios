#include "applayer.h"
#include "debuglayer.h"
#include "pauselayer.h"

#include <entt/entity/fwd.hpp>
#include <iostream>
#include <memory>
#include <pscore/application.h>
#include <raylib.h>

#include <psinterfaces/movable.h>
#include <psinterfaces/renderable.h>

#include <entt/entt.hpp>
#include <thread>
#include <vector>

class FunnyRectangle : public PSInterfaces::IMovable, PSInterfaces::IRenderable
{
public:
	FunnyRectangle()
	{
	}

	void render() override
	{
		Rectangle rect{m_pos.x, m_pos.y, 200, 200};
		DrawRectanglePro(rect, {100, 100}, m_rot, RED);
	};

	void move(const float dt) override
	{
		if ( m_pos.x >= GetScreenWidth() - 100 || m_pos.x <= 0 + 100 )
			m_vel.x *= -1;

		if ( m_pos.y >= GetScreenHeight() - 100 || m_pos.y <= 0 + 100 )
			m_vel.y *= -1;

		m_pos.x += m_vel.x * dt;
		m_pos.y += m_vel.y * dt;
	}
};

class AppLayerPriv
{
	friend class AppLayer;

	// entt::registry entity_registry;
	FunnyRectangle* fn_rec;
};

AppLayer::AppLayer()
{
	_p = std::make_unique<AppLayerPriv>();

	// auto ent = _p->entity_registry.create();
	// _p->entity_registry.emplace<>(ent, Args && args...);
	_p->fn_rec = new FunnyRectangle();
	_p->fn_rec->set_pos({GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f});
	_p->fn_rec->set_vel({100, 200});
	_p->fn_rec->set_rot(0);
}

AppLayer::~AppLayer()
{
}

void AppLayer::on_update(const float dt)
{
	{ // TODO: Refactor this scope out of the App layer
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

	if ( !active )
		return;

	_p->fn_rec->move(dt);
}

void AppLayer::on_render(const float dt)
{
	_p->fn_rec->render();
}
