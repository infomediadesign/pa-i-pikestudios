#include <memory>
#include <pscore/application.h>
#include <pscore/collision.h>
#include <psinterfaces/entity.h>
#include <psinterfaces/layer.h>
#include <raylib.h>
#include <raymath.h>
#include <utility>
#include <vector>

namespace PSCore {
	namespace collision {
		std::optional<std::pair<const std::weak_ptr<PSInterfaces::IEntity>, Vector2>>
		check_entity_collision(const PSInterfaces::IEntity* self, const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entities)
		{
			for ( const auto& entity: entities ) {
				auto locked_entity = entity.lock();
				if ( !locked_entity || locked_entity->uid() == self->uid() || !locked_entity->position().has_value() ||
					 !self->position().has_value() || !locked_entity->bounds().has_value() || !self->bounds().has_value() )
					continue;

				auto pos1	  = locked_entity->position().value();
				auto pos2	  = self->position().value();
				auto distance = Vector2Length(Vector2Subtract(pos1, pos2));
				if ( distance > 50 )
					continue;

				for ( const Vector2& point: self->bounds().value() ) {
					auto bounds = locked_entity->bounds().value();
					if ( CheckCollisionPointPoly(point, bounds.data(), bounds.size()) ) {
						PS_LOG(LOG_INFO, TextFormat(
												 "%s (%f, %f) collided with %s (%f, %f)", self->ident().data(), self->position()->x, self->position()->y,
												 locked_entity->ident().data(), locked_entity->position()->x, locked_entity->position()->y
										 ));
						return std::make_pair(locked_entity, point);
					}
				}
			}

			return std::nullopt;
		}
	} // namespace collision
} // namespace PSCore
