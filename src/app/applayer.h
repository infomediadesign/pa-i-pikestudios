#pragma once

#include <memory>
#include <psinterfaces/layer.h>

class AppLayerPriv;
class AppLayer : public PSInterfaces::Layer
{
public:
	AppLayer();
	~AppLayer() override;

	void on_update(const float dt) override;
	void on_render() override;

private:
	std::unique_ptr<AppLayerPriv> _p;
};
