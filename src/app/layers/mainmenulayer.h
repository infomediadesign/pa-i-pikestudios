#pragma once

#include <psinterfaces/layer.h>

class MainMenuLayer : public PSInterfaces::Layer
{
	public:
		void on_update(float dt) override;
		
		void on_render() override;
};