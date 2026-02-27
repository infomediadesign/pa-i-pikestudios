#include "debuglayer.h"

#include <cstdint>
#include <imgui.h>
#include <pscore/application.h>
#include <pscore/viewport.h>
#include <rlImGui.h>
#include "layers/applayer.h"

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

	ImGui::GetIO().ConfigDebugHighlightIdConflicts = false;

	ImGui::Begin("Game Debug");

	ImGui::Text("%s", TextFormat("CURRENT FPS: %i", static_cast<int64_t>(1.0f / m_dt)));

	ImGui::Checkbox("Draw all", &m_draw_all);

	if ( ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None) ) {

		if ( m_draw_all || ImGui::BeginTabItem("Direktor") ) {
			if ( auto director = gApp()->game_director() )
				director->draw_debug();

			if ( !m_draw_all )
				ImGui::EndTabItem();
		}

		if ( m_draw_all || ImGui::BeginTabItem("Viewport") ) {
			if ( auto& viewport = gApp()->viewport() )
				viewport->draw_debug();

			if ( !m_draw_all )
				ImGui::EndTabItem();
		}

		if ( m_draw_all || ImGui::BeginTabItem("Lighting") ) {
			if ( auto* sun = gApp()->sunlight_shader() )
				sun->draw_debug();

			if ( !m_draw_all )
				ImGui::EndTabItem();
		}

		if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
			for ( auto entity: app_layer->entities() ) {
				if ( auto locked_entity = entity.lock() ) {
					if ( m_draw_all || ImGui::BeginTabItem(locked_entity->ident().c_str()) ) {
						locked_entity->draw_debug();

						if ( !m_draw_all )
							ImGui::EndTabItem();
					}
				}
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	rlImGuiEnd();
}
