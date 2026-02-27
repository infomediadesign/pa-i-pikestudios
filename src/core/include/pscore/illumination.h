#pragma once

#include <imgui.h>
#include <raylib.h>

class SunLight
{
public:
	Shader shader;

	Vector3 direction = {0.4f, -0.3f, 0.8f};
	Vector3 color	  = {1.0f, 0.95f, 0.85f};
	float intensity	  = 0.8f;

	Vector3 ambient_color	= {0.4f, 0.4f, 0.5f};
	float ambient_intensity = 0.7f;

	float spec_intensity = 0.0f;
	float spec_power	 = 16.0f;

	void update_shader()
	{
		SetShaderValue(shader, GetShaderLocation(shader, "sun_direction"), &direction, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, GetShaderLocation(shader, "sun_color"), &color, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, GetShaderLocation(shader, "sun_intensity"), &intensity, SHADER_UNIFORM_FLOAT);
		SetShaderValue(shader, GetShaderLocation(shader, "ambient_color"), &ambient_color, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader, GetShaderLocation(shader, "ambient_intensity"), &ambient_intensity, SHADER_UNIFORM_FLOAT);
		SetShaderValue(shader, GetShaderLocation(shader, "spec_intensity"), &spec_intensity, SHADER_UNIFORM_FLOAT);
		SetShaderValue(shader, GetShaderLocation(shader, "spec_power"), &spec_power, SHADER_UNIFORM_FLOAT);
	}

	void draw_debug()
	{
		bool changed = false;

		ImGui::Text("Sun Light");
		ImGui::Separator();

		// --- Sun Direction ---
		ImGui::Text("Direction");
		float dir[3] = {direction.x, direction.y, direction.z};
		if ( ImGui::SliderFloat3("##sun_dir", dir, -1.0f, 1.0f, "%.2f") ) {
			direction = {dir[0], dir[1], dir[2]};
			changed	  = true;
		}

		// --- Sun Color ---
		ImGui::Text("Sun Color");
		float sc[3] = {color.x, color.y, color.z};
		if ( ImGui::ColorEdit3("##sun_color", sc) ) {
			color	= {sc[0], sc[1], sc[2]};
			changed = true;
		}

		// --- Sun Intensity ---
		if ( ImGui::SliderFloat("Sun Intensity", &intensity, 0.0f, 3.0f, "%.2f") ) {
			changed = true;
		}

		ImGui::Separator();
		ImGui::Text("Ambient");

		// --- Ambient Color ---
		float ac[3] = {ambient_color.x, ambient_color.y, ambient_color.z};
		if ( ImGui::ColorEdit3("##ambient_color", ac) ) {
			ambient_color = {ac[0], ac[1], ac[2]};
			changed		  = true;
		}

		// --- Ambient Intensity ---
		if ( ImGui::SliderFloat("Ambient Intensity", &ambient_intensity, 0.0f, 2.0f, "%.2f") ) {
			changed = true;
		}

		ImGui::Separator();
		ImGui::Text("Specular");

		// --- Specular Intensity ---
		if ( ImGui::SliderFloat("Spec Intensity", &spec_intensity, 0.0f, 2.0f, "%.2f") ) {
			changed = true;
		}

		// --- Specular Power ---
		if ( ImGui::SliderFloat("Spec Power", &spec_power, 1.0f, 128.0f, "%.1f") ) {
			changed = true;
		}

		if ( changed ) {
			update_shader();
		}
	}

	~SunLight()
	{
		UnloadShader(shader);
	}
};