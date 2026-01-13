#pragma once

#include <memory>
#include <psinterfaces/entity.h>
#include <psinterfaces/layer.h>
#include <raylib.h>
#include <type_traits>
#include <vector>
#include "pscore/viewport.h"

namespace PSCore {

	template<typename TL>
	concept ILayerDerived = std::is_base_of_v<PSInterfaces::Layer, TL>;

	class ApplicationPriv;
	class Application
	{
	public:
		using WindowSize = Vector2;
		struct AppSpec
		{
			const char* title;
			WindowSize size;
		};

		Application(const AppSpec& spec);
		~Application();

		static Application* get();

		/*!
		 * @brief pushes a Layer of type T to the layer stack
		 */
		template<ILayerDerived TL>
		void push_layer()
		{
			m_layer_stack.push_back(std::make_unique<TL>());
		};

		/*!
		 * @brief returns an instance to a layer of type T or a nullptr
		 */
		template<ILayerDerived TL>
		TL* get_layer()
		{
			for ( const auto& layer: m_layer_stack ) {
				if ( auto casted = dynamic_cast<TL*>(layer.get()) )
					return casted;
			}
			return nullptr;
		}

		/*!
		 * @brief removes every occourence of type T from the layer stack
		 */
		template<ILayerDerived TL>
		void pop_layer()
		{
			for ( auto itr = m_layer_stack.begin(); itr != m_layer_stack.end(); ) {
				if ( auto casted = dynamic_cast<TL*>(itr->get()) )
					itr = m_layer_stack.erase(itr);
				else
					itr++;
			}
		};

		/*!
		 * @brief starts the update/render loop
		 */
		void run();

		/*!
		 * @brief stops the applikation
		 */
		void stop();

		/*!
		 * @brief registers an entity to the applikation
		 * @param a shared pointer to an entity derived class
		 */
		template<typename E>
			requires std::is_base_of_v<PSInterfaces::IEntity, E>
		void register_entity(std::shared_ptr<E> e)
		{
			if ( auto entity = dynamic_cast<PSInterfaces::IEntity*>(e.get()) )
				m_entity_registry.push_back(e);
		};

		/*!
		 * @brief returns a list of weak_ptr of all registered entiteies
		 * @return the list of registere entities
		 */
		std::vector<std::weak_ptr<PSInterfaces::IEntity>> entities() const;

		/*!
		 * @brief prints a log message, use the PS_LOG macro instead
		 * @param type: a loglevel
		 * @param text: the log message
		 */
		void log(TraceLogLevel type, const char* text) const;

		/*!
		 * @brief removes an entity from the registry
		 * @param e: a shared pointer to an entity derived class
		 */
		template<typename E>
			requires std::is_base_of_v<PSInterfaces::IEntity, E>
		void unregister_entity(std::shared_ptr<E> e)
		{
			for ( auto itr = m_entity_registry.begin(); itr != m_entity_registry.end(); ) {
				if ( auto locked = itr->lock() ) {
					if ( locked.get() == e.get() ) {
						itr = m_entity_registry.erase(itr);
						return;
					}
					++itr;
				} else {
					itr = m_entity_registry.erase(itr);
				}
			}
		}

		PSCore::Viewport* viewport();

	private:
		std::unique_ptr<ApplicationPriv> _p;
		std::vector<std::unique_ptr<PSInterfaces::Layer>> m_layer_stack;

		std::vector<std::weak_ptr<PSInterfaces::IEntity>> m_entity_registry;
	};
} // namespace PSCore

constexpr auto gApp = PSCore::Application::get;

#define PS_LOG(log_level, msg)                                                                                                                       \
	gApp()->log(log_level, std::string(std::string(__FILE__) + " LINE: " + std::to_string(__LINE__) + " MSG: " + msg).data());
