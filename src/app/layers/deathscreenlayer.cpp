//
// Created by rikab on 29/01/2026.
//
#include <pscore/application.h>
#include <raygui.h>
#include <layers/deathscreenlayer.h>
#include <pscore/viewport.h>

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

	float w = 128*sk;
	float x = (static_cast<float>(GetScreenWidth()) / 2.0f) - (w/2);
	Rectangle rect{np.x+x*sk, np.y+48*sk, w, 40*sk};
	GuiLabel(rect, "Du bist gestorben :(");
}
