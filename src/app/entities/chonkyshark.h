#pragma once

#include <entities/shark.h>

class ChonkyShark : public Shark
{
public:
	ChonkyShark();
	~ChonkyShark();

	void update(float dt) override;

	std::optional<std::vector<Vector2>> bounds() const override;

	void on_hit() override;

	void set_is_active(bool active) override;

private:
	bool m_hurt = false;
	bool m_uwu	= false;

	float m_iframe_duration		  = CFG_VALUE<float>("chonky_shark_iframe_duration", 0.2f);
	float m_remaining_iframe_time = 0.0f;

	bool determine_uwu_();
};
