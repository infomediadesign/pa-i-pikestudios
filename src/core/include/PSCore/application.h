#pragma once

#include <memory>
#include <psinterfaces/layer.h>
#include <raylib.h>
#include <type_traits>
#include <vector>

namespace PSCore {

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

		template<typename TL>
			requires(std::is_base_of_v<PSInterfaces::Layer, TL>)
		void push_layer()
		{
			m_layer_stack.push_back(std::make_unique<TL>());
		};

		template<typename TL>
			requires(std::is_base_of_v<PSInterfaces::Layer, TL>)
		TL* get_layer()
		{
			for ( const auto& layer: m_layer_stack ) {
				if ( auto casted = dynamic_cast<TL*>(layer.get()) )
					return casted;
			}
			return nullptr;
		}

		template<typename TL>
			requires(std::is_base_of_v<PSInterfaces::Layer, TL>)
		void pop_layer()
		{
			for (auto itr = m_layer_stack.begin(); itr != m_layer_stack.end();) {
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
