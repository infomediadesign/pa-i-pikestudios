//
// Created by rikab on 29/01/2026.
//
#pragma once 
#include <psinterfaces/layer.h>

class DeathScreenLayer : public PSInterfaces::Layer
{
	public:
		void on_update(float dt) override;
		void on_render() override;
};


