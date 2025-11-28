#pragma once

#include <psinterfaces/layer.h>

class DebugLayer : public PSInterfaces::Layer
{
	public:
	DebugLayer();
	~DebugLayer();
	
	void on_update(const int dt) override;
	void on_render(const int dt) override;
	
	private:
	
};