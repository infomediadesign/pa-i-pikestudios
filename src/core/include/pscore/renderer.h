#include <algorithm>
#include <functional>
#include <memory>
#include <psinterfaces/events.h>
#include <psinterfaces/renderable.h>
#include <vector>

namespace PSCore {

	template<typename T>
	concept IRenderableDerived = std::is_base_of_v<PSInterfaces::IRenderable, T>;

	/*!
	 * @details A Renderer used for rendering Renderables with a specified z index
	 */
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

		/*!
		 * @brief submits a renderable to the renderer, the renderer injects its event mnager and listens for z index changes
		 * @param renderable: a Shared Pointer to a renderable
		 */
		template<IRenderableDerived T>
		void submit_renderable(std::shared_ptr<T> renderable)
		{
			if ( auto* b = dynamic_cast<PSInterfaces::IRenderable*>(renderable.get()) )
				m_render_queue.push_back(renderable);

			if ( auto rb = m_render_queue.back().lock() )
				rb->add_event_manager(m_events.get());

			sort_();
		}

		/*!
		 * @brief removes a submitted renderable
		 * @param renderable: a Shared Pointer to a renderable
		 */
		template<IRenderableDerived T>
		void remove_rendarble(std::shared_ptr<T> renderable)
		{
			for ( auto itr = m_render_queue.begin(); itr != m_render_queue.end(); ) {
				auto temp_renderable = itr->lock(); // create accses to the renderable for further use
				if ( auto casted = dynamic_cast<T*>(temp_renderable.get()) ) {
					// If the casted renderable is the one passed
					if ( renderable.get() == casted ) {
						// remove the submitted event manager
						temp_renderable->remove_event_manager(m_events.get());
						// and remove it from the list
						itr = m_render_queue.erase(itr);
						return;
					}
				}

				++itr;
			}
		}

		/*!
		 * @brief calls the render funktion of every submitted renderable
		 */
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
			// Sort the renderables on the basis of their z_index
			std::sort(
					m_render_queue.begin(), m_render_queue.end(),
					[](const std::weak_ptr<PSInterfaces::IRenderable>& r1, const std::weak_ptr<PSInterfaces::IRenderable>& r2) {
						return r1.lock()->z_index() < r2.lock()->z_index();
					}
			);
		}
	};
} // namespace PSCore
