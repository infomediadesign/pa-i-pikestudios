//
// Created by rikab on 29/01/2026.
//
#include <pscore/application.h>
#include <raygui.h>
#include <layers/deathscreenlayer.h>
#include <layers/mainmenulayer.h>
#include <pscore/viewport.h>
#include <entities/director.h>

void DeathScreenLayer::on_update(float dt)
{
	if ( IsKeyPressed(KEY_ENTER)) {
		std::string name_input = "Wer";
			//= m_score_layer_instance->player_name_input;
		m_last_input_name = name_input;
		printf("Enter pressed\n");
		printf("%s\n", m_last_input_name.c_str());
			//m_score_layer_instance->player_name_input;
	}
	//printf("%s\n", m_last_input_name);
}

void DeathScreenLayer::on_render()
{
	auto& vp = gApp()->viewport();
	Vector2 np = vp->viewport_origin();
	float  sk = vp->viewport_scale();

	Color bg_clr{0,0,0, 150};
	DrawRectangle(np.x, np.y, GetScreenWidth()*sk, GetScreenHeight()*sk, bg_clr);

	float w = 300;
	float x = ((vp->viewport_base_size().x) / 2.0f) - (w/2);
	Rectangle rect{np.x+x*sk, np.y+48*sk, w*sk, 40*sk};

	std::string bounty_text = std::to_string(
			dynamic_cast<FortunaDirector*>(gApp()->game_director())->m_b_bounty.bounty());
	Rectangle score{np.x+x*sk, np.y+100*sk, w*sk, 40*sk};
	Rectangle score_info_bounds{np.x + x * sk, np.y + 125 * sk, w * sk, 40 * sk};
	Rectangle name_input_bounds{np.x + x * sk, np.y + 150 * sk, w * sk, 40 * sk};

	int oldColor = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
	int oldSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
	int oldAlign = GuiGetStyle(LABEL, TEXT_ALIGNMENT);

	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xff0000ff);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 28 * sk);

	GuiLabel(rect, "Du bist gestorben :(");

	GuiSetStyle(DEFAULT, TEXT_SIZE, 14 * sk);

	GuiLabel(score, ("Score: " + bounty_text).c_str());
	if ( m_score_should_be_saved ) {
		GuiLabel(score_info_bounds, "Du hast es in die Top 10 geschafft!");
		GuiLabel(name_input_bounds, ("Name: " + m_score_layer_instance->player_name_input).c_str());
	} 
	else {
		GuiLabel(score_info_bounds, "Du hast es nicht unter die Top 10 geschafft");
	}
	

	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, oldColor);
	GuiSetStyle(DEFAULT, TEXT_SIZE, oldSize);
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, oldAlign);

	int margin = 20;
	int btn_height = 24;
	int btn_width = 80;

	float y = (vp->viewport_base_size().y - btn_height) - margin;

	if (GuiButton(Rectangle{ np.x + margin*sk, np.y + y*sk ,btn_width*sk, btn_height*sk }, "Mainmenu")) {
		gApp()->call_later([]() { gApp()->pop_layer<DeathScreenLayer>(); });
		gApp()->call_later([]() { gApp()->switch_layer<AppLayer, MainMenuLayer>(); });
	}

	x = (vp->viewport_base_size().x - btn_width) - margin;

	if (GuiButton(Rectangle{ np.x + x*sk, np.y + y*sk, btn_width*sk, btn_height*sk }, "Retry")){
		gApp()->call_later([]() { gApp()->pop_layer<AppLayer>(); });
		gApp()->call_later([]() { gApp()->pop_layer<ScoreLayer>(); });
		m_score_layer_instance->set_layer_is_visible(true);
		gApp()->call_later([]() { gApp()->switch_layer<DeathScreenLayer, AppLayer>(); });
		gApp()->call_later([]() { gApp()->game_director_ref().reset(new FortunaDirector()); });
	}
}

void DeathScreenLayer::set_score_should_be_saved(bool should_be_saved)
{
	m_score_should_be_saved = should_be_saved;
}

void DeathScreenLayer::set_score_layer_instance(ScoreLayer* score_layer)
{
	m_score_layer_instance = score_layer;
}

bool DeathScreenLayer::score_should_be_saved() const
{
	return m_score_should_be_saved;
}

void DeathScreenLayer::set_last_input_name(const std::string& name)
{
	m_last_input_name = name;
}

std::string DeathScreenLayer::last_input_name() const
{
	return m_last_input_name;
}
