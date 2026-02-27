#pragma once

#include <pscore/collision.h>
#include <pscore/settings.h>
#include <pscore/sprite.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <vector>

class HunterPriv;
class Hunter : public PSInterfaces::IRenderable
{
public:
	Hunter();
	~Hunter();

	void update(float dt) override;
	void render() override;
	void draw_debug() override;
	void on_hit() override;

	void init(std::shared_ptr<Hunter> self);

	std::optional<Vector2> position() const override;

	virtual std::optional<float> rotation() const override;

	virtual std::optional<Vector2> velocity() const override;

	std::optional<std::vector<Vector2>> bounds() const override;

	std::optional<Vector2> size() const override;
	
	void set_is_active(bool active) override;

	enum State { Patrolling = 0, Wreck };

private:
	std::unique_ptr<HunterPriv> _p;
	
	std::pair<Vector2, Vector2> gen_path_egde();
	std::vector<Vector2> gen_patrol_path();
	
	void traverse_path_(float dt);
	bool player_in_range_(Vector2* to_player);
	void fire_available_cannon_(bool right_side, float dt);
	void avoid_other_hunters_(float dt);
	float gen_phase_offset_();
};
