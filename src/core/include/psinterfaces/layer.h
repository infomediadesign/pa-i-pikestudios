#pragma once

#include <memory>
#include <pscore/renderer.h>
#include <pscore/utils.h>
#include <psinterfaces/entity.h>
#include <psinterfaces/renderable.h>
#include <raylib.h>
#include <vector>

namespace PSInterfaces {
	class Layer
	{
	public:
		explicit Layer();
		virtual ~Layer() = default;

		virtual void on_event() {};
		virtual void on_update(float dt) = 0;
		virtual void on_render()		 = 0;

		void suspend();

		void resume();

		const std::unique_ptr<PSCore::Renderer>& renderer();

		/*!
		 * @brief registers an entity to the applikation
		 * @param a shared pointer to an entity derived class
		 */
		template<typename E>
			requires std::is_base_of_v<PSInterfaces::IEntity, E>
		void register_entity(std::shared_ptr<E> e, bool submit_render = false)
		{
			if ( auto entity = dynamic_cast<PSInterfaces::IEntity*>(e.get()) )
				entities_.push_back(e);

			if ( !submit_render )
				return;

			if ( auto renderable = dynamic_cast<PSInterfaces::IRenderable*>(e.get()) )
				renderer_->submit_renderable(e);
		};

		/*!
		 * @brief removes an entity from the registry
		 * @param e: a shared pointer to an entity derived class
		 */
		template<typename E>
			requires std::is_base_of_v<PSInterfaces::IEntity, E>
		void unregister_entity(std::shared_ptr<E> e)
		{
			for ( auto itr = entities_.begin(); itr != entities_.end(); ) {
				if ( auto locked = itr->lock() ) {
					if ( locked.get() == e.get() ) {
						renderer_->remove_rendarble(e);
						itr = entities_.erase(itr);
						return;
					}
					++itr;
				} else {
					renderer_->remove_rendarble(e);
					itr = entities_.erase(itr);
				}
			}
		}

		std::vector<std::weak_ptr<PSInterfaces::IEntity>>& entities();
		
		std::vector<std::weak_ptr<PSInterfaces::IEntity>> entities() const;

	private:
		std::vector<std::weak_ptr<PSInterfaces::IEntity>> entities_;

	protected:
		const std::unique_ptr<PSCore::Renderer> renderer_ = std::make_unique<PSCore::Renderer>();

		bool active = true;
	};
} // namespace PSInterfaces
