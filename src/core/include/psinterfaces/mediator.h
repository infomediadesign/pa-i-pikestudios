#pragma once

#include <raylib.h>
#include <string>
#include <variant>

namespace PSInterfaces {

	class IEntity;

	class IMediator
	{
	public:
		~IMediator() = default;

		struct Event
		{
			std::string name;
			std::variant<Vector2, int, float, std::string> val;
		};

		virtual void notify(const IEntity* sender, const Event& event) = 0;
	};
} // namespace PSInterfaces
