#include "debuglayer.h"

#include <cstdint>
#include <imgui.h>
#include <rlImGui.h>
#include <pscore/application.h>
#include <pscore/viewport.h>

DebugLayer::DebugLayer()
{
	rlImGuiSetup(true);
}

DebugLayer::~DebugLayer()
{
}

void DebugLayer::on_update(const float dt)
{
	m_dt = dt;
}

void DebugLayer::on_render()
{
	rlImGuiBeginDelta(m_dt);

#ifdef IMGUI_HAS_DOCK
	ImGui::DockSpaceOverViewport(
			0, NULL, ImGuiDockNodeFlags_PassthruCentralNode
	); // set ImGuiDockNodeFlags_PassthruCentralNode so that we can see the raylib contents behind the dockspace
#endif

	ImGui::Begin("Game Debug");

	ImGui::Text("%s", TextFormat("CURRENT FPS: %i", static_cast<int64_t>(1.0f / m_dt)));

	if ( auto director = gApp()->game_director() )
		director->draw_debug();

	if ( auto& viewport = gApp()->viewport() )
		viewport->draw_debug();

	for ( auto entity: gApp()->entities() ) {
		if ( auto locked_entity = entity.lock() ) {
			locked_entity->draw_debug();
		}
	}

	ImGui::End();

	rlImGuiEnd();
}
