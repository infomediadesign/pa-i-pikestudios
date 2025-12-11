#pragma once

#include <memory>
#include <psinterfaces/layer.h>
#include <raylib.h>
#include <type_traits>
#include <vector>

namespace PSCore {

	template<typename TL>
	concept ILayerDerived = std::is_base_of_v<PSInterfaces::Layer, TL>;

	class ApplicationPriv;
	class Application
	{
	public:
		typedef Vector2 WindowSize;
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
			if ( !get_layer<TL>() )
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

	private:
		std::unique_ptr<ApplicationPriv> _p;
		std::vector<std::unique_ptr<PSInterfaces::Layer>> m_layer_stack;
	};
} // namespace PSCore
