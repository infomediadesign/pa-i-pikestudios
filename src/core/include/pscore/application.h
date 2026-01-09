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
			requires std::is_base_of_v<PSInterfaces::IEntity, E>
		void register_entity(std::shared_ptr<E> e)
		{
			if ( auto entity = dynamic_cast<PSInterfaces::IEntity*>(e.get()) )
				m_entity_registry.push_back(e);
		};

		std::vector<std::weak_ptr<PSInterfaces::IEntity>> entities() const;

		void log(TraceLogLevel type, const char* text) const;

	private:
		std::unique_ptr<ApplicationPriv> _p;
		std::vector<std::unique_ptr<PSInterfaces::Layer>> m_layer_stack;

		std::vector<std::weak_ptr<PSInterfaces::IEntity>> m_entity_registry;
	};
} // namespace PSCore

constexpr auto gApp = PSCore::Application::get;

#define PS_LOG(log_level, msg) gApp()->log(log_level, std::string( std::string(__FILE__) \
								+ " LINE: " + std::to_string(__LINE__) + " MSG: " + msg ).data() );
