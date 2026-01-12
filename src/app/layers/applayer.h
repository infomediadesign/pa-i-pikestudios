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

	// Functions to spawn and destroy players
	std::shared_ptr<Player> spawn_player(const Vector2& position);
	void destroy_player(std::shared_ptr<Player> player);
	void sync_player_entities();

private:
	std::unique_ptr<AppLayerPriv> _p;
};
