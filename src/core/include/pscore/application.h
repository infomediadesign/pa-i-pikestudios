#pragma once

#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <psinterfaces/layer.h>
#include <raylib.h>
#include <type_traits>
#include <vector>

namespace PSCore {

	class Viewport;
	namespace sprites {
		class SpriteLoader;
	}

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
			const char* icon_path;
			WindowSize size;
		};

		Application();
		~Application();

		void init(const AppSpec& spec);

		static Application* get();

		/*!
		 * @brief pushes a Layer of type T to the layer stack
		 */
		template<ILayerDerived TL>
		void push_layer()
		{
			m_layer_stack.push_back(std::make_unique<TL>());
		}

		template<ILayerDerived TL, ILayerDerived TO>
		void switch_layer()
		{
			for ( auto itr = m_layer_stack.begin(); itr != m_layer_stack.end(); itr++) {
				if ( auto casted = dynamic_cast<TL*>(itr->get()) ) {
					itr->reset(new TO());
					return;
				}
			}
		}

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
		 * @brief prints a log message, use the PS_LOG macro instead
		 * @param type: a loglevel
		 * @param text: the log message
		 */
		void log(TraceLogLevel type, const char* text) const;

		template<typename E>
			requires std::is_base_of_v<PSInterfaces::IEntity, E>
		void add_game_director()
		{
			m_game_director = std::make_unique<E>();
		};

		PSInterfaces::IEntity* game_director()
		{
			if ( m_game_director )
				return m_game_director.get();

			return nullptr;
		};

		std::unique_ptr<PSInterfaces::IEntity>& game_director_ref()
		{
			return m_game_director;
		};

		std::unique_ptr<PSCore::Viewport>& viewport();

		std::unique_ptr<PSCore::sprites::SpriteLoader>& sprite_loader();

		void call_later(std::function<void()> fn)
		{
			m_call_stack.push_back(fn);
		}

		void set_current_player_name(std::string& name);
		std::string current_player_name();

		float delta_time();

	private:
		std::unique_ptr<ApplicationPriv> _p;
		std::vector<std::unique_ptr<PSInterfaces::Layer>> m_layer_stack;
		std::unique_ptr<PSInterfaces::IEntity> m_game_director;
		std::deque<std::function<void()>> m_call_stack;
		std::string m_current_player_name;
	};
} // namespace PSCore

constexpr auto gApp = PSCore::Application::get;

#define PS_LOG(log_level, msg)                                                                                                                       \
	gApp()->log(log_level, std::string(std::string(__FILE__) + " LINE: " + std::to_string(__LINE__) + " MSG: " + msg).data());
