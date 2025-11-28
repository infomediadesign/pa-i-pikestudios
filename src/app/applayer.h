#pragma once

#include <memory>
#include <psinterfaces/layer.h>

class AppLayerPriv;
class AppLayer : public PSInterfaces::Layer
{
public:
	AppLayer();
	~AppLayer() override;

	void on_update(const int dt) override;
	void on_render(const int dt) override;

private:
	std::unique_ptr<AppLayerPriv> _p;
};
