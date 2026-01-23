#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

struct Vector2;

namespace PSInterfaces {
	class IEntity;
	class Layer;
} // namespace PSInterfaces

namespace PSCore {
	namespace collision {

		class EntityCollider
		{
		public:
			explicit EntityCollider(std::weak_ptr<PSInterfaces::IEntity> parent);

			void register_collision_handler(std::function<void(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> cb);

			bool check_collision(
					const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entity_pool,
					std::function<bool(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> exclusion_criterion =
							[](std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point) { return true; }
			);

			static std::optional<std::pair<const std::weak_ptr<PSInterfaces::IEntity>, Vector2>>
			check_entity_collision(const PSInterfaces::IEntity* self, const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entities);

		private:
			std::weak_ptr<PSInterfaces::IEntity> m_parent;
			std::function<void(std::weak_ptr<PSInterfaces::IEntity> other, const Vector2& point)> m_collion_cb;
		};
	} // namespace collision
} // namespace PSCore
