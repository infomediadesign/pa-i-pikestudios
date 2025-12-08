#pragma once

#include <psinterfaces/layer.h>

class DebugLayer : public PSInterfaces::Layer
{
	public:
	DebugLayer();
	~DebugLayer();
	
	void on_update(const float dt) override;
	void on_render() override;
	
	private:
	
};