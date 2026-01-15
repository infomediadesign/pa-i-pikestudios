#pragma once

#include <memory>

#include <entities/player.h>
#include <psinterfaces/entity.h>

class FortunaDirectorPriv;
class FortunaDirector : public PSInterfaces::IEntity
{
public:
	FortunaDirector();
	~FortunaDirector();

	void update(float dt) override;
	
	void draw_debug() override;

	// Functions to spawn and destroy players
	std::shared_ptr<Player> spawn_player(const Vector2& position);
	void destroy_player(std::shared_ptr<Player> player);
	void sync_player_entities();
private:
	std::unique_ptr<FortunaDirectorPriv> _p;

};
