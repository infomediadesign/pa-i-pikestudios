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

	void set_can_open_pause_menu(bool can_open);
	bool can_open_pause_menu() const;

private:
	std::unique_ptr<AppLayerPriv> _p;
	bool m_can_open_pause_menu = true;
};
