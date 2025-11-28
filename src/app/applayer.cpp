#include "applayer.h"
#include "debuglayer.h"

#include <memory>
#include <pscore/application.h>
#include <raylib.h>

#include <psinterfaces/movable.h>
#include <psinterfaces/renderable.h>

#include <entt/entt.hpp>

class FunnyRectangle : public PSInterfaces::IMovable, PSInterfaces::IRenderable
{
	public:
	FunnyRectangle() {
		
	}
	
	void render() override {
		Rectangle rect{m_pos.x, m_pos.y, 50, 50};
		DrawRectanglePro(rect, {25, 25}, m_rot, RED);
	};
	
};

class AppLayerPriv
{
	friend class AppLayer;
};

AppLayer::AppLayer()
{
	_p = std::make_unique<AppLayerPriv>();
}

AppLayer::~AppLayer()
{
	
}

void AppLayer::on_update(const int dt)
{
	if ( IsKeyPressed(KEY_F3) ) {
		auto app = PSCore::Application::get();
		if ( app->get_layer<DebugLayer>() )
			app->pop_layer<DebugLayer>();
		else
			app->push_layer<DebugLayer>();
	}
}

void AppLayer::on_render(const int dt)
{
}
