//
// Created by rikab on 29/01/2026.
//
#include <pscore/application.h>
#include <raygui.h>
#include <layers/deathscreenlayer.h>
#include <pscore/viewport.h>
#include <entities/director.h>

void DeathScreenLayer::on_update(float dt)
{
	
	
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

	int oldColor = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
	int oldSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
	int oldAlign = GuiGetStyle(LABEL, TEXT_ALIGNMENT);

	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xff0000ff);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 28 * sk);

	GuiLabel(rect, "Du bist gestorben :(");
	GuiLabel(score, ("Score: " + bounty_text).c_str());

	GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, oldColor);
	GuiSetStyle(DEFAULT, TEXT_SIZE, oldSize);
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, oldAlign);
}
