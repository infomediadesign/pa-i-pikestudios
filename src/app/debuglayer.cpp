#include "debuglayer.h"

#include <cstdint>
#include <imgui.h>
#include <rlImGui.h>

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
	ImGui::End();

	rlImGuiEnd();
}
