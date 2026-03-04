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
	ExplosiveBarrel();
	~ExplosiveBarrel();

	void update(float dt) override;

	void render() override;

	void draw_debug() override;

	void set_is_active(bool active) override;
	
	void init(std::shared_ptr<ExplosiveBarrel> self, const Vector2& position);

	std::optional<std::vector<Vector2>> bounds() const override;
	
	std::optional<Vector2> position() const override { return m_position; };
	
	void set_parent(std::shared_ptr<PSInterfaces::IEntity> parent) { m_parent = parent; }

private:
	std::unique_ptr<PSCore::collision::EntityCollider> m_collider;
	PSCore::sprites::SpriteSheetAnimation m_animation_controller;
	std::shared_ptr<PSInterfaces::IEntity> m_parent;

	float m_explosion_radius = CFG_VALUE<float>("explosive_barrel_explosion_radius", 100.0f);

	float m_damage_time		= CFG_VALUE<float>("explosive_barrel_damage_time", 0.8f);
	float m_detonation_time = CFG_VALUE<float>("explosive_barrel_detonation_time", 3.0f);
	float m_timer			= 0.0f;

	Vector2 m_position{0.0f, 0.0f};

	// Blinking, Creeper awww man
	Shader m_flash_shader	 = LoadShader(NULL, "resources/shader/sprite_flash.fs");
	Vector4 m_flash_color	 = {255, 0, 0, 255};
	float m_flash_lerp_scale = 18;
	float m_flash_alpha		 = 0;
	int m_flash_alpha_location;

	std::vector<Vector2> generate_circle_polygon_(Vector2 center, float radius, int segments = 36) const;
	bool is_exploding() const;

	// Sound
	Sound m_explode_sound = LoadSound("resources/sfx/bomb_explode.mp3");

	float m_global_sfx_volume = 0;
	float m_shoot_volume = 1;
	float m_shoot_pitch = 0.75;

	Vector2 m_volume_boundary = {-20,20};
	Vector2 m_pitch_boundary = {-40,40};

	bool m_can_play_sound = true;
};
