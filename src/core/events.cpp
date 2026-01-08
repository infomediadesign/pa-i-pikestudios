#include <psinterfaces/entity.h>
#include <psinterfaces/events.h>

void PSInterfaces::Events::IEventManager::notify(const Event& event) const
{
	auto specific_listers = event_listeners_.equal_range(event.name);
	for ( auto it = specific_listers.first; it != specific_listers.second; ++it ) {
		it->second(event);
	}
};

void PSInterfaces::Events::IEventManager::subscribe(const std::string& event, const notify_callback& cb)
{
	event_listeners_.insert({event, cb});
}
