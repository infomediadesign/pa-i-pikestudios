#include <layers/contributorslayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/application.h>
#include <pscore/sprite.h>
#include <pscore/viewport.h>
#include <raygui.h>
#include <raylib.h>
#include <string>
#include <vector>

static const Color COLOR_NAME	 = {101, 57, 24, 255};
static const Color COLOR_ROLE	 = {80, 55, 30, 200};
static const Color COLOR_SECTION = {50, 30, 10, 255};

static const float SECTION_TITLE_SIZE = 16.0f;
static const float NAME_SIZE		  = 8.0f;
static const float ROLE_SIZE		  = 6.0f;
static const float ENTRY_SPACING	  = 6.0f;
static const float SECTION_SPACING	  = 24.0f;
static const float COLUMN_GAP		  = 20.0f;
static const float PAPER_PADDING_X	  = 200.0f;

static const char* PROJECT_GITHUB_URL = "https://github.com/infomediadesign/pa-i-pikestudios";
static const char* PROJECT_ITCH_URL	  = "https://pikestudios.itch.io/fortunas-echo";

ContributorsLayer::ContributorsLayer() : PSInterfaces::Layer()
{
	Vector2 frame_grid{1, 1};
	m_button	  = PRELOAD_TEXTURE("smallbutton", "resources/ui/button_small.png", frame_grid)->m_s_texture;
	m_pike_logo	  = PRELOAD_TEXTURE("pike_logo", "resources/ui/logo.png", frame_grid)->m_s_texture;
	m_background  = PRELOAD_TEXTURE("background", "resources/ui/credits_layer.png", frame_grid)->m_s_texture;
	m_link_button = PRELOAD_TEXTURE("smallbuttonlong", "resources/ui/button_small_long.png", frame_grid)->m_s_texture;


	m_name_font = LoadFontEx("resources/fonts/fax_font.ttf", 256, nullptr, 0);
	SetTextureFilter(m_name_font.texture, TEXTURE_FILTER_BILINEAR);
	SetTextureFilter(m_name_font.texture, TEXTURE_FILTER_BILINEAR);

	m_sections = {
			{"Pike Studios",
			 {{"Noah Felber", "Programming Lead"},
			  {"Manuel Bender", "Programming & Art"},
			  {"Noel Knauf", "Programming"},
			  {"Cali Helmstaedter", "Art Lead"},
			  {"Rika Brenner", "Design Lead & Art"},
			  {"Tylor Arnold", "Design"},
			  {"Marius Wolframm", "Design, Sound & Prog."}}}
	};
}

ContributorsLayer::~ContributorsLayer()
{
}

void ContributorsLayer::on_update(float dt)
{
	if ( IsKeyPressed(KEY_ESCAPE) ) {
		gApp()->call_later([]() { gApp()->switch_layer<ContributorsLayer, MainMenuLayer>(); });
		gApp()->play_ui_sound(0);
		return;
	}
}

float ContributorsLayer::draw_credits(const Vector2& anchor, float scale)
{
	float spacing	 = 1.0f * scale;
	float title_size = SECTION_TITLE_SIZE * scale;
	float name_size	 = NAME_SIZE * scale;
	float role_size	 = ROLE_SIZE * scale;
	float entry_h	 = name_size + role_size + 2.0f * scale + ENTRY_SPACING * scale;
	float col_gap	 = COLUMN_GAP * scale;

	// Center the paper on screen
	auto& vp			= gApp()->viewport();
	Vector2 screen_size = vp->viewport_base_size();
	float paper_w		= (float) m_background.width * scale;
	float paper_h		= (float) m_background.height * scale;
	float paper_x		= anchor.x + (screen_size.x * scale - paper_w) / 2.0f;
	float paper_y		= anchor.y;

	float paper_inner_left	= paper_x + PAPER_PADDING_X * scale;
	float paper_inner_width = paper_w - PAPER_PADDING_X * 2.0f * scale;
	float single_col_w		= (paper_inner_width - col_gap) / 2.0f;

	// Logo centered on the paper
	float logo_scale	 = scale / 8.0f;
	float logo_w		 = m_pike_logo.width * logo_scale;
	float logo_h		 = m_pike_logo.height * logo_scale;
	float paper_center_x = paper_x + paper_w / 2.0f;
	DrawTextureEx(m_pike_logo, {paper_center_x - logo_w / 2.0f, paper_y + (screen_size.y * scale) - 60.0f * scale}, 0, logo_scale, WHITE);

	float cur_y = paper_y + 20.0f * scale + logo_h + 16.0f * scale;

	for ( const auto& section: m_sections ) {
		// Section title
		DrawTextEx(m_name_font, section.title.c_str(), {paper_inner_left, cur_y}, title_size, spacing, COLOR_SECTION);
		cur_y += title_size + 8.0f * scale;

		// Separator line
		DrawLineEx({paper_inner_left, cur_y}, {paper_inner_left + paper_inner_width, cur_y}, 1.0f * scale, {80, 55, 30, 80});
		cur_y += 8.0f * scale;

		if ( !section.members.empty() ) {
			int total	   = static_cast<int>(section.members.size());
			int left_count = (total + 1) / 2;

			// Left column
			float left_y = cur_y;
			for ( int i = 0; i < left_count; i++ ) {
				DrawTextEx(m_name_font, section.members[i].name.c_str(), {paper_inner_left, left_y}, name_size, spacing, COLOR_NAME);
				DrawTextEx(
						m_name_font, section.members[i].role.c_str(), {paper_inner_left, left_y + name_size + 2.0f * scale}, role_size, spacing,
						COLOR_ROLE
				);
				left_y += entry_h;
			}

			// Right column
			float right_x = paper_inner_left + single_col_w + col_gap;
			float right_y = cur_y;
			for ( int i = left_count; i < total; i++ ) {
				DrawTextEx(m_name_font, section.members[i].name.c_str(), {right_x, right_y}, name_size, spacing, COLOR_NAME);
				DrawTextEx(
						m_name_font, section.members[i].role.c_str(), {right_x, right_y + name_size + 2.0f * scale}, role_size, spacing, COLOR_ROLE
				);
				right_y += entry_h;
			}

			cur_y += left_count * entry_h;
		}

		cur_y += SECTION_SPACING * scale;
	}

	return cur_y;
}

void ContributorsLayer::on_render()
{
	if ( auto& vp = gApp()->viewport() ) {

		Vector2 anchor		= vp->viewport_origin();
		float scale			= vp->viewport_scale();
		Vector2 screen_size = vp->viewport_base_size();

		DrawTextureEx(m_background, anchor, 0, scale, WHITE);

		float btn_width				 = static_cast<float>(m_button.width);
		float btn_height			 = static_cast<float>(m_button.height);
		float button_boarder_padding = 20;

		float button_pos_y = screen_size.y - btn_height / 2.0f - button_boarder_padding;

		// Draw credits content on the paper
		float credits_end_y = draw_credits(anchor, scale);

		// --- Main Menu button ---
		GuiSetFont(m_name_font);
		GuiSetStyle(DEFAULT, TEXT_SIZE, 10 * scale);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt({0, 0, 0, 255}));

		Vector2 mainmenu_pos = {anchor.x / scale + button_boarder_padding + btn_width / 2.0f, anchor.y / scale + button_pos_y};
		if ( GuiButtonTexture(m_button, mainmenu_pos, 0, scale, WHITE, GRAY, "Main Menu") ) {
			gApp()->call_later([]() { gApp()->switch_layer<ContributorsLayer, MainMenuLayer>(); });
			gApp()->play_ui_sound(0);
		}

		float link_btn_width  = static_cast<float>(m_link_button.width);
		float link_btn_height = static_cast<float>(m_link_button.height);
		float link_btn_gap	  = 10.0f;

		// Center the two link buttons on the X axis, directly below the name block
		float total_links_width = link_btn_width * 2.0f + link_btn_gap;
		float links_center_x	= anchor.x / scale + screen_size.x / 2.0f;
		float links_base_y		= credits_end_y / scale + link_btn_height / 2.0f;

		// GitHub button (left)
		Vector2 github_pos = {links_center_x - total_links_width / 2.0f + link_btn_width / 2.0f, links_base_y};
		if ( GuiButtonTexture(m_link_button, github_pos, 0, scale, WHITE, GRAY, "GitHub") ) {
			OpenURL(PROJECT_GITHUB_URL);
		}

		// itch.io button (right)
		Vector2 itch_pos = {links_center_x + total_links_width / 2.0f - link_btn_width / 2.0f, links_base_y};
		if ( GuiButtonTexture(m_link_button, itch_pos, 0, scale, WHITE, GRAY, "itch.io") ) {
			OpenURL(PROJECT_ITCH_URL);
		}
	}
}
