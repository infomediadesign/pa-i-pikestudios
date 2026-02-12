#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <pscore/spawner.h>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <psinterfaces/entity.h>

namespace PSInterfaces {
	class IEntity;
	class Layer;
} // namespace PSInterfaces

namespace PSCore {
	namespace collision {

		template<class O,class E, class L>
			requires std::is_base_of_v<PSInterfaces::IEntity, E> && std::is_base_of_v<PSInterfaces::Layer, L>
		Vector2 entity_repel_force(std::shared_ptr<O> self, const Spawner<E, L>& spawner, float min_distance, float repel_strenght)
		{
			std::shared_ptr<PSInterfaces::IEntity> origin_entity;
			if ( !(origin_entity = std::dynamic_pointer_cast<PSInterfaces::IEntity>(self)) )
				return {0, 0};

			Vector2 repel_force{0, 0};
			for ( const auto& entity: spawner.entities() ) {
				if ( std::shared_ptr<PSInterfaces::IEntity> other_entity = std::dynamic_pointer_cast<PSInterfaces::IEntity>(entity) ) {
					if ( !other_entity->is_active() || other_entity->uid() == origin_entity->uid() )
						continue;

					Vector2 other_pos = other_entity->position().value_or(Vector2{0, 0});
					Vector2 self_pos  = origin_entity->position().value_or(Vector2{0, 0});

					Vector2 diff   = Vector2Subtract(self_pos, other_pos);
					float distance = Vector2Distance(self_pos, other_pos);

					if ( distance < min_distance && distance > 0 ) {
						Vector2 direction	 = Vector2Normalize(diff);
						float overlap		 = min_distance - distance;
						float forceMagnitude = overlap * repel_strenght;

						repel_force = Vector2Add(repel_force, Vector2Scale(direction, forceMagnitude));
					}
				}
			}
			return repel_force;
		}

		class EntityCollider
		{
		public:
			explicit EntityCollider(std::weak_ptr<PSInterfaces::IEntity> parent);

			void register_collision_handler(std::function<void(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> cb, float timeout = 0);

			bool check_collision(
					const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entity_pool,
					std::function<bool(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> exclusion_criterion =
							[](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point) { return true; }
			);

			static std::optional<std::pair<const std::weak_ptr<PSInterfaces::IEntity>, Vector2>>
			check_entity_collision(std::shared_ptr<PSInterfaces::IEntity> self, const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entities);

		private:
			std::weak_ptr<PSInterfaces::IEntity> m_parent;
			std::function<void(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> m_collion_cb;
			std::chrono::steady_clock::time_point cb_last_called = std::chrono::steady_clock::now();
			
			float m_collision_cb_timeout = 0;
		};
	} // namespace collision
} // namespace PSCore
