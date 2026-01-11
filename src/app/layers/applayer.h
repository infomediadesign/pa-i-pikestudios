#pragma once

#include <memory>
#include <psinterfaces/layer.h>

#include <entities/player.h>

class AppLayerPriv;
class AppLayer : public PSInterfaces::Layer
{
public:
	AppLayer();
	~AppLayer() override;

	void on_update(const float dt) override;
	void on_render() override;

	// WARNING: DO NOT DO THIS.. this is only a temporary solution to try things out. A layer should not be responsible for entites
	std::shared_ptr<Player> spawn_player(const Vector2& position);

private:
	std::unique_ptr<AppLayerPriv> _p;
};
