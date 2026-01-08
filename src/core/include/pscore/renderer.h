#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <psinterfaces/events.h>
#include <psinterfaces/renderable.h>
#include <vector>

namespace PSCore {

	template<typename T>
	concept IRenderableDerived = std::is_base_of_v<PSInterfaces::IRenderable, T>;

	class Renderer
	{
	public:
		Renderer()
		{
			m_events->subscribe("propose_z_index", std::bind(&Renderer::handle_event_, this, std::placeholders::_1));
		};
		~Renderer()
		{
			for ( auto renderable: m_render_queue ) {
				if ( auto locked_renderable = renderable.lock() )
					locked_renderable->remove_event_manager(m_events.get());
			}
		};

		template<IRenderableDerived T>
		void submit_renderable(std::shared_ptr<T> renderable)
		{
			if ( auto* b = dynamic_cast<PSInterfaces::IRenderable*>(renderable.get()) )
				m_render_queue.push_back(renderable);

			if ( auto rb = m_render_queue.back().lock() ) {
				rb->add_event_manager(m_events.get());
			}

			sort_();
		}

		template<IRenderableDerived T>
		void remove_rendarble(std::shared_ptr<T> renderable)
		{
			for ( auto itr = m_render_queue.begin(); itr != m_render_queue.end(); ) {
				auto temp_renderable = itr->lock();
				if ( auto casted = dynamic_cast<T*>(temp_renderable.get()) ) {
					if ( renderable.get == casted ) {
						temp_renderable->remove_event_manager(m_events.get());
						itr = m_render_queue.erase(itr);
						return;
					}
				}

				++itr;
			}
		}

		void render()
		{
			for ( auto itr = m_render_queue.begin(); itr != m_render_queue.end(); ) {
				if ( auto r_locked = itr->lock() ) {
					r_locked->render();
					++itr;
				} else // renderable is expired; we dont need it
					itr = m_render_queue.erase(itr);
			}
		}

	private:
		std::vector<std::weak_ptr<PSInterfaces::IRenderable>> m_render_queue;
		std::unique_ptr<PSInterfaces::Events::IEventManager> m_events = std::make_unique<PSInterfaces::Events::IEventManager>();

		void handle_event_(const PSInterfaces::Events::Event& event)
		{
			if ( event.name == "propose_z_index" )
				sort_();
		}

		void sort_()
		{
			std::sort(
					m_render_queue.begin(), m_render_queue.end(),
					[](const std::weak_ptr<PSInterfaces::IRenderable>& r1, const std::weak_ptr<PSInterfaces::IRenderable>& r2) {
						return r1.lock()->z_index() < r2.lock()->z_index();
					}
			);
		}
	};
} // namespace PSCore
