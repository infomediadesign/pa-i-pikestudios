#pragma once

#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <pscore/application.h>
#include <pscore/utils.h>
#include <psinterfaces/entity.h>
#include <psinterfaces/layer.h>
#include <vector>

namespace PSCore {

	template<class T, ILayerDerived L>
		requires std::is_base_of_v<PSInterfaces::IEntity, T>
	class Spawner : public PSInterfaces::IEntity
	{
	public:
		Spawner(std::chrono::duration<double> interval, int variation = 0, int limit = 100) : PSInterfaces::IEntity("spawner")
		{
			m_interval	= interval;
			m_variation = variation;
			m_limit		= limit;
		};
		~Spawner() {};

		void update(float dt) override
		{
			std::chrono::duration<double> variation{static_cast<double>(PSUtils::gen_rand(m_variation * -1, m_variation))};
			if ( !m_running || (std::chrono::steady_clock::now() - m_last_time) < (m_interval + variation) )
				return;

			spawn();
		};

		void resume()
		{
			m_running = true;
		}

		void suspend()
		{
			m_running = false;
		}

		void spawn()
		{
			if ( m_entities.size() >= m_limit )
				return;

			if ( auto layer = dynamic_cast<PSInterfaces::Layer*>(gApp()->get_layer<L>()) ) {
				if ( m_custom_spawn && m_custom_spawn(layer) )
					return;

				const auto is_suspended = [](std::shared_ptr<T> entity) -> bool {
					if ( std::shared_ptr<PSInterfaces::IEntity> casted = std::dynamic_pointer_cast<PSInterfaces::IEntity>(entity) ) {
						return !casted->is_active();
					}
				};

				std::shared_ptr<T> entity;

				auto itr = std::find_if(m_entities.begin(), m_entities.end(), is_suspended);
				if ( itr != m_entities.end() ) {
					entity = (*itr);
					entity->set_is_active(true);
				} else {
					entity = std::make_shared<T>();
					m_entities.push_back(entity);
					layer->register_entity(entity, true);
				}
				if ( m_spawn_cb )
					m_spawn_cb(entity);
			}

			m_last_time = std::chrono::steady_clock::now();
		}

		void despawn(std::shared_ptr<T> entity, bool suspend = true)
		{
			if ( suspend ) {
				if ( std::shared_ptr<PSInterfaces::IEntity> casted = std::dynamic_pointer_cast<PSInterfaces::IEntity>(entity) ) {
					casted->set_is_active(false);
					return;
				}
			}

			if ( auto layer = dynamic_cast<PSInterfaces::Layer*>(gApp()->get_layer<L>()) ) {
				layer->unregister_entity(entity);
			}
			m_entities.erase(entity);
		}

		void register_spawn_callback(std::function<void(std::shared_ptr<T> entity)> cb)
		{
			m_spawn_cb = cb;
		}

		void register_custom_spawn(std::function<bool(PSInterfaces::Layer* layer)> spawn)
		{
			m_custom_spawn = spawn;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_last_time = std::chrono::steady_clock::now();
		std::function<bool(PSInterfaces::Layer* layer)> m_custom_spawn;
		std::function<void(std::shared_ptr<T> entity)> m_spawn_cb;
		std::vector<std::shared_ptr<T>> m_entities;
		std::chrono::duration<double> m_interval{100};
		int m_variation = 0;
		int m_limit		= 10;

		bool m_running = false;
	};
} // namespace PSCore
