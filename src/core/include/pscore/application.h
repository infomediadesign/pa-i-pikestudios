#pragma once

#include <memory>
#include <psinterfaces/layer.h>
#include <raylib.h>
#include <type_traits>
#include <vector>
#include "psinterfaces/entity.h"

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

		template<ILayerDerived TL>
		void push_layer()
		{
			m_layer_stack.push_back(std::make_unique<TL>());
		};

		template<ILayerDerived TL>
		TL* get_layer()
		{
			for ( const auto& layer: m_layer_stack ) {
				if ( auto casted = dynamic_cast<TL*>(layer.get()) )
					return casted;
			}
			return nullptr;
		}

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

		void run();
		void stop();

		template<typename E>
			requires std::is_pointer_v<E>
		void register_entity(E e)
		{
			if ( auto entity = dynamic_cast<PSInterfaces::IEntity*>(e) )
				m_entity_registry.push_back(entity);
		};

		const std::vector<PSInterfaces::IEntity*> entities() const
		{
			return m_entity_registry;
		}

	private:
		std::unique_ptr<ApplicationPriv> _p;
		std::vector<std::unique_ptr<PSInterfaces::Layer>> m_layer_stack;

		std::vector<PSInterfaces::IEntity*> m_entity_registry;
	};
} // namespace PSCore

constexpr auto gApp = PSCore::Application::get;
