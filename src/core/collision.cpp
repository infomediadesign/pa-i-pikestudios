#include <algorithm>
#include <cerrno>
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

		void EntityCollider::register_collision_handler(
				std::function<void(std::vector<std::weak_ptr<PSInterfaces::IEntity>> others, const Vector2& point)> cb, float timeout
		)
		{
			m_collision_cb_timeout = timeout;
			m_collion_cb		   = cb;
		}

		bool EntityCollider::check_collision(
				const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entity_pool,
				std::function<bool(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> exclusion_criterion
		)
		{
			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

			std::vector<std::weak_ptr<PSInterfaces::IEntity>> collided_entities;
			collided_entities.reserve(entity_pool.size());
			
			Vector2 point;
			if ( auto locked = m_parent.lock() ) {
				auto vals = check_entity_collision(locked, entity_pool);

				for ( const auto& val: vals.value_or({}) ) {
					auto other = val.first;
					point	   = val.second;
					if ( exclusion_criterion(other, point) ) {
						collided_entities.push_back(other);
					}
				}
			}

			std::chrono::duration<float> elapsed = now - cb_last_called;
			if ( elapsed.count() >= m_collision_cb_timeout ) {
				if ( m_collion_cb ) {
					collided_entities.shrink_to_fit();
					m_collion_cb(collided_entities, point);
					cb_last_called = now;
				}
			}

			return collided_entities.size() > 0;
		}

		std::optional<std::vector<std::pair<const std::weak_ptr<PSInterfaces::IEntity>, Vector2>>> EntityCollider::check_entity_collision(
				std::shared_ptr<PSInterfaces::IEntity> self, const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entities
		)
		{
			std::vector<std::pair<const std::weak_ptr<PSInterfaces::IEntity>, Vector2>> collided_entities;
			collided_entities.reserve(entities.size());
			
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
					collided_entities.push_back(pair.value());
				if ( auto pair = check_col(self, locked_entity, true) )
					collided_entities.push_back(pair.value());
			}

			collided_entities.shrink_to_fit();
			if ( collided_entities.size() > 0 )
				return collided_entities;

			return std::nullopt;
		}
	} // namespace collision
} // namespace PSCore
