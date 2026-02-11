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

		EntityCollider::EntityCollider(std::weak_ptr<PSInterfaces::IEntity> parent)
		{
			m_parent = parent;
		}

		void EntityCollider::register_collision_handler(std::function<void(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> cb)
		{
			m_collion_cb = cb;
		}

		bool EntityCollider::check_collision(
				const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entity_pool,
				std::function<bool(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> exclusion_criterion
		)
		{
			if ( auto locked = m_parent.lock() ) {
				if ( auto val = check_entity_collision(locked, entity_pool) ) {
					auto other = val.value().first;
					auto point = val.value().second;
					if ( m_collion_cb && exclusion_criterion(other, point) ) {
						m_collion_cb(other, point);
						return true;
					}
				}
			}

			return false;
		}

		std::optional<std::pair<const std::weak_ptr<PSInterfaces::IEntity>, Vector2>> EntityCollider::check_entity_collision(
				std::shared_ptr<PSInterfaces::IEntity> self, const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entities
		)
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

				const auto check_col = [](std::shared_ptr<PSInterfaces::IEntity> en1, std::shared_ptr<PSInterfaces::IEntity> en2,
										  bool inverted = false) -> std::optional<std::pair<std::weak_ptr<PSInterfaces::IEntity>, Vector2>> {
					for ( const Vector2& point: en1->bounds().value_or({}) ) {
						auto bounds = en2->bounds().value_or({});
						if ( CheckCollisionPointPoly(point, bounds.data(), bounds.size()) )
							return std::make_pair(inverted ? en2 : en1, point);
					}

					return std::nullopt;
				};

				if ( auto pair = check_col(locked_entity, self) )
					return pair;
				if ( auto pair = check_col(self, locked_entity, true) )
					return pair;
			}

			return std::nullopt;
		}
	} // namespace collision
} // namespace PSCore
