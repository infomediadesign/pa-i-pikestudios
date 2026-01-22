#pragma once

#include <optional>
#include <psinterfaces/events.h>
#include <string>
#include <vector>
#include <raylib.h>
#include "pscore/utils.h"

namespace PSInterfaces {

	namespace Events {
		struct Event;
		class IEventManager;
	}; // namespace Events

	class IEntity
	{
	public:
		explicit IEntity(const std::string& ident) : ident_(ident) {};
		virtual ~IEntity() = default;

		virtual void update(float dt) = 0;

		virtual void draw_debug() {};

		virtual std::optional<std::vector<Vector2>> bounds() const
		{
			return std::nullopt;
		};

		virtual std::optional<Vector2> position() const
		{
			return std::nullopt;
		}

		virtual bool is_active() const
		{
			return true;
		}

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

		const std::string ident() const
		{
			return ident_;
		}
		
		const std::string uid() const {
			return uid_;
		}

	protected:
		std::vector<const Events::IEventManager*> event_managers_;
		const std::string ident_;
		const std::string uid_ = PSUtils::generate_uid();
	};
} // namespace PSInterfaces
