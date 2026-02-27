#pragma once

#include <raylib.h>
#include <imgui.h>
#include <cmath>

/*!
 * @brief Utility class for rendering drop shadows beneath 2D sprites.
 *
 * Each entity that wants to cast a shadow creates a ShadowCaster instance.
 * Before rendering the actual sprite, call `render_shadow(...)` to draw
 * a flattened, tinted silhouette offset in the direction opposite the sun.
 *
 * The shadow shader (`drop_shadow.fs`) is shared across all casters and
 * loaded once via the static `init` / `shutdown` methods. Call `init()`
 * after the raylib window is created, and `shutdown()` before closing it.
 */
class ShadowCaster
{
public:
	// ── Static lifetime management ──────────────────────────────────────

	static void init()
	{
		if (s_ref_count == 0)
		{
			s_shader = LoadShader(nullptr, "resources/shader/drop_shadow.fs");
			s_loc_shadow_color = GetShaderLocation(s_shader, "shadow_color");
		}
		++s_ref_count;
	}

	static void shutdown()
	{
		if (s_ref_count > 0)
			--s_ref_count;

		if (s_ref_count == 0)
			UnloadShader(s_shader);
	}

	static Shader shader() { return s_shader; }

	// ── Instance ────────────────────────────────────────────────────────

	ShadowCaster()
	{
		init();
	}

	~ShadowCaster()
	{
		shutdown();
	}

	// Non-copyable, movable
	ShadowCaster(const ShadowCaster&) = delete;
	ShadowCaster& operator=(const ShadowCaster&) = delete;
	ShadowCaster(ShadowCaster&& other) noexcept
		: shadow_color(other.shadow_color),
		  shadow_offset_scale(other.shadow_offset_scale),
		  shadow_squash_y(other.shadow_squash_y),
		  enabled(other.enabled)
	{
		++s_ref_count;
	}
	ShadowCaster& operator=(ShadowCaster&&) = delete;

	/*!
	 * @brief Renders a drop shadow for the given sprite.
	 *
	 * Call this BEFORE rendering the actual sprite so the shadow appears beneath it.
	 *
	 * @param texture          The sprite's diffuse texture.
	 * @param source           Source rectangle on the texture (animation frame).
	 * @param position         World-space position of the sprite (viewport coords).
	 * @param rotation         Sprite rotation in degrees.
	 * @param sun_direction    The sun direction vector (points FROM surface TO sun).
	 *                         The shadow is cast in the opposite direction.
	 * @param viewport_origin  The viewport origin offset.
	 * @param viewport_scale   The current viewport scale factor.
	 */
	void render_shadow(
		const Texture2D& texture,
		const Rectangle& source,
		const Vector2& position,
		float rotation,
		const Vector3& sun_direction,
		const Vector2& viewport_origin,
		float viewport_scale
	) const
	{
		if (!enabled)
			return;

		// Compute the 2D shadow offset from the sun direction.
		// The shadow falls opposite to where the sun is shining from.
		// sun_direction points TO the sun, so the shadow goes in -XY.
		// The Z component controls how "high" the sun is; higher Z = shorter shadow.
		float sun_z = fmaxf(sun_direction.z, 0.05f); // avoid division by zero / very long shadows
		float offset_x = (-sun_direction.x / sun_z) * shadow_offset_scale;
		float offset_y = (-sun_direction.y / sun_z) * shadow_offset_scale;

		// Shadow destination rectangle: same size as sprite but offset
		float dest_w = fabsf(source.width) * viewport_scale;
		float dest_h = fabsf(source.height) * viewport_scale * shadow_squash_y;

		Rectangle dest = {
			viewport_origin.x + (position.x + offset_x) * viewport_scale,
			viewport_origin.y + (position.y + offset_y) * viewport_scale,
			dest_w,
			dest_h
		};

		Vector2 origin = { dest_w / 2.0f, dest_h / 2.0f };

		// Upload shadow color uniform
		float color_vec[4] = { shadow_color.x, shadow_color.y, shadow_color.z, shadow_color.w };
		SetShaderValue(s_shader, s_loc_shadow_color, color_vec, SHADER_UNIFORM_VEC4);

		BeginShaderMode(s_shader);
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
		EndShaderMode();
	}

	// ── Tweakable parameters ────────────────────────────────────────────

	/// Shadow color (RGB) and opacity (A), all in 0-1 range.
	Vector4 shadow_color = { 0.0f, 0.0f, 0.05f, 0.15f };

	/// How far the shadow is cast (in world/viewport units).
	/// Higher values = longer shadow. Tune to taste based on your sprite sizes.
	float shadow_offset_scale = 12.0f;

	/// Vertical squash factor to give the shadow a perspective feel.
	/// 1.0 = same height as sprite, < 1.0 = flattened.
	float shadow_squash_y = 0.85f;

	/// Master toggle.
	bool enabled = true;

	// ── Debug UI ────────────────────────────────────────────────────────

	void draw_debug()
	{
		ImGui::Text("Shadow Caster");
		ImGui::Separator();

		ImGui::Checkbox("Enabled##shadow", &enabled);

		float sc[4] = { shadow_color.x, shadow_color.y, shadow_color.z, shadow_color.w };
		if (ImGui::ColorEdit4("Shadow Color##shadow", sc))
			shadow_color = { sc[0], sc[1], sc[2], sc[3] };

		ImGui::SliderFloat("Offset Scale##shadow", &shadow_offset_scale, 0.0f, 50.0f, "%.1f");
		ImGui::SliderFloat("Squash Y##shadow", &shadow_squash_y, 0.1f, 1.0f, "%.2f");
	}

private:
	// ── Shared static state ─────────────────────────────────────────────
	static inline Shader s_shader = { 0 };
	static inline int s_loc_shadow_color = -1;
	static inline int s_ref_count = 0;
};