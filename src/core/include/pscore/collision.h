#pragma once

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
		std::optional<std::pair<const std::weak_ptr<PSInterfaces::IEntity>, Vector2>>
		check_entity_collision(const PSInterfaces::IEntity* self, const std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entities);
	}
} // namespace PSCore
