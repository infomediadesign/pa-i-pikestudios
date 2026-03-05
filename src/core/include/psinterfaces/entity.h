#pragma once

#include <optional>
#include <psinterfaces/events.h>
#include <raylib.h>
#include <string>
#include <vector>
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

		virtual std::optional<Vector2> size() const
		{
			return std::nullopt;
		}

		virtual std::optional<Vector2> position() const
		{
			return std::nullopt;
		}

		virtual std::optional<Vector2> velocity() const
		{
			return std::nullopt;
		}

		virtual std::optional<float> rotation() const
		{
			return std::nullopt;
		}

		virtual bool is_active() const
		{
			return is_active_;
		}

		virtual void set_is_active(bool active)
		{
			is_active_ = active;
		}

		virtual bool is_dead_hitable() const
		{
			return is_dead_hitable_;
		}

		virtual void set_is_dead_hitable(bool active)
		{
			is_dead_hitable_ = active;
		}

		virtual bool is_check_valid() const
		{
			return is_check_valid_;
		}

		virtual void set_is_check_valid(bool active)
		{
			is_check_valid_ = active;
		}

		virtual void on_hit()
		{
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

		const uint64_t uid() const
		{
			return uid_;
		}

	protected:
		std::vector<const Events::IEventManager*> event_managers_;
		const std::string ident_;
		const uint64_t uid_ = PSUtils::generate_uid();

		bool is_active_		  = true;
		bool is_dead_hitable_ = false;
		bool is_check_valid_	  = true;
	};
} // namespace PSInterfaces
