#pragma once

#include <vector>
#include <psinterfaces/events.h>

namespace PSInterfaces {

	namespace Events {
		struct Event;
		class IEventManager;
	}; // namespace Events

	class IEntity
	{
	public:
		~IEntity() = default;

		virtual void update(float dt) = 0;

		void add_event_manager(const Events::IEventManager* manager)
		{
			if ( std::find(event_managers_.begin(), event_managers_.end(), manager) == event_managers_.end() ) {
				event_managers_.push_back(manager);
			}
		}

		void remove_event_manager(const Events::IEventManager* manager)
		{
			event_managers_.erase(std::remove(event_managers_.begin(), event_managers_.end(), manager), event_managers_.end());
		}

		void notify_all(const Events::Event& event)
		{
			for ( auto manager: event_managers_ )
				manager->notify(event);
		}

	protected:
		std::vector<const Events::IEventManager*> event_managers_;
	};
} // namespace PSInterfaces
