#pragma once

#include <psinterfaces/layer.h>
#include <raylib.h>
#include <string>
#include <vector>

struct Contributor
{
	std::string name;
	std::string role;
};

struct TeamSection
{
	std::string title;
	std::vector<Contributor> members;
};

class ContributorsLayer : public PSInterfaces::Layer
{
public:
	ContributorsLayer();
	~ContributorsLayer();

	void on_update(float dt) override;
	void on_render() override;

private:
	float draw_credits(const Vector2& anchor, float scale);

	Texture2D m_button;
	Texture2D m_pike_logo;
	Texture2D m_background;
	Texture2D m_link_button;

	Font m_name_font;

	std::vector<TeamSection> m_sections;
};