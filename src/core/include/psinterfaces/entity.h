#pragma once

namespace PSInterfaces {
	class IEntity {
		public:
		~IEntity() = default;
		
		virtual void update(float dt) = 0;
	};
}