#include <layers/applayer.h>
#include <memory>
#include <optional>
#include <pscore/collision.h>
#include <pscore/settings.h>
#include <pscore/sprite.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <vector>

class ExplosiveBarrel : public PSInterfaces::IRenderable
{
public:
	ExplosiveBarrel() : PSInterfaces::IEntity("explosive_barrel")
	{
		SetShaderValue(m_flash_shader, GetShaderLocation(m_flash_shader, "flash_color"), &m_flash_color, SHADER_UNIFORM_VEC4);
		m_flash_alpha_location = GetShaderLocation(m_flash_shader, "flash_alpha");
	};
	~ExplosiveBarrel();

	void update(float dt) override
	{
		if ( m_timer += dt >= m_detonation_time ) {
			if ( m_timer > m_detonation_time + m_damage_time )
				set_is_active(false);

			if ( auto app_layer = gApp()->get_layer<AppLayer>() ) {
				m_collider->check_collision(app_layer->entities());
			}

			return;
		}

		float n_flash_lerp_scale = m_flash_lerp_scale / m_detonation_time;
		m_flash_alpha			 = 0.5f * cosf(m_timer * sqrtf(m_timer) * n_flash_lerp_scale * sqrtf(n_flash_lerp_scale)) + 0.5;

		SetShaderValue(m_flash_shader, m_flash_alpha_location, &m_flash_alpha, SHADER_UNIFORM_FLOAT);
	};

	void render() override
	{
		BeginShaderMode(m_flash_shader);
		if ( m_timer <= m_detonation_time ) {
			DrawCircleLines(m_position.x, m_position.y, m_explosion_radius, RED);
		}
		EndShaderMode();

		DrawCircleV(m_position, 20, WHITE);
	};

	void draw_debug() override;

	void set_is_active(bool active) override
	{
		is_active_ = active;

		if ( is_active_ ) {
			m_timer		  = 0;
			m_flash_alpha = 0;
		}
	};

	void set_position(const Vector2& position)
	{
		m_position = position;
	}

	void init(std::shared_ptr<ExplosiveBarrel> self, const Vector2& position)
	{
		m_position = position;

		m_collider = std::make_unique<PSCore::collision::EntityCollider>(self);
		m_collider->register_collision_handler([this](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& pos) {
			if ( auto locked = other.lock() ) {
				locked->on_hit();
			}
		});
	}

	std::optional<std::vector<Vector2>> bounds() const override
	{
		if ( is_active_ )
			return generate_circle_polygon_(m_position, m_explosion_radius);

		return std::nullopt;
	}

private:
	std::unique_ptr<PSCore::collision::EntityCollider> m_collider;
	PSCore::sprites::SpriteSheetAnimation m_animation_controller;

	float m_explosion_radius = CFG_VALUE<float>("explosive_barrel_explosion_radius", 100.0f);

	float m_damage_time		= CFG_VALUE<float>("explosive_barrel_damage_time", 0.5f);
	float m_detonation_time = CFG_VALUE<float>("explosive_barrel_detonation_time", 3.0f);
	float m_timer			= 0.0f;

	Vector2 m_position{0.0f, 0.0f};

	// Blinking, Creeper awww man
	Shader m_flash_shader	 = LoadShader(NULL, "resources/shader/sprite_flash.fs");
	Vector4 m_flash_color	 = {255, 0, 0, 255};
	float m_flash_lerp_scale = 6;
	float m_flash_alpha		 = 0;
	int m_flash_alpha_location;

	std::vector<Vector2> generate_circle_polygon_(Vector2 center, float radius, int segments = 36) const
	{
		std::vector<Vector2> points;
		points.reserve(segments);

		const float angleStep = 2.0f * PI / segments;

		for ( int i = 0; i < segments; ++i ) {
			float angle = i * angleStep;
			points.push_back({center.x + radius * std::cos(angle), center.y + radius * std::sin(angle)});
		}

		return points;
	}
};
