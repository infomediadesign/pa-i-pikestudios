#pragma once

#include <psinterfaces/layer.h>
#include <raylib.h>

class PauseLayer: public PSInterfaces::Layer
{
	public:
	PauseLayer();
	
	void on_update(float dt) override;
	void on_render() override;

	bool m_active = true;
};