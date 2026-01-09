#pragma once

#include <functional>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <variant>

namespace PSInterfaces {

	class IEntity;

	namespace Events {

		struct Event
		{
			std::string name;
			std::variant<Vector2, int, float, std::string> val;
		};

		class IEventManager
		{
		public:
			~IEventManager() = default;

			virtual void notify(const Events::Event& event) const;

			using notify_callback = std::function<void(const Events::Event& event)>;
			void subscribe(const std::string& event, const notify_callback& cb);

		protected:
			std::unordered_map<std::string, notify_callback> event_listeners_;
		};
	} // namespace Events
} // namespace PSInterfaces
