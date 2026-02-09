#pragma once

#include <algorithm>
#include <chrono>
#include <functional>
#include <imgui.h>
#include <iterator>
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
		Spawner(float interval, int variation = 0, int limit = 100, bool render = true) : PSInterfaces::IEntity("spawner")
		{
			m_interval	= interval;
			m_variation = variation;
			m_limit		= limit;
			m_render	= render;

			m_interval_left = interval + PSUtils::gen_rand(m_variation * -1, m_variation);
		};
		~Spawner() {};

		void draw_debug() override
		{
			ImGui::Separator();
			ImGui::Text("%s Spawner", typeid(T).name());

			ImGui::SliderFloat("Spawn Interval", &m_interval, 0.1, 1000);
			ImGui::SliderInt("Spawn Variation", &m_variation, 0, m_interval - 1);
			if ( ImGui::SliderInt("Spawn Cap", &m_limit, 1, 200) ) {
				despawn_if_limit_exceeded_();
			}
		}

		void update(float dt) override
		{
			m_interval_left -= dt;
			if ( !m_running || m_interval_left > 0 )
				return;

			m_interval_left = m_interval + PSUtils::gen_rand(m_variation * -1, m_variation);

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
			if ( count_active_() >= m_limit )
				return;

			if ( auto layer = dynamic_cast<PSInterfaces::Layer*>(gApp()->get_layer<L>()) ) {
				if ( m_custom_spawn && m_custom_spawn(layer) )
					return;

				std::shared_ptr<T> entity;

				auto itr = std::find_if(m_entities.begin(), m_entities.end(), is_suspended_);
				if ( itr != m_entities.end() ) {
					entity = (*itr);
					entity->set_is_active(true);
				} else {
					entity = std::make_shared<T>();
					m_entities.push_back(entity);
					layer->register_entity(entity, m_render);
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

		void set_interval(float interval)
		{
			m_interval = interval;
		}
		float interval()
		{
			return m_interval;
		}

		void set_variation(int variation)
		{
			m_variation = variation;
		}
		int variation()
		{
			return m_variation;
		}

		void set_limit(int limit)
		{
			m_limit = limit;
			despawn_if_limit_exceeded_();
		}
		int limit()
		{
			return m_limit;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_last_time = std::chrono::steady_clock::now();
		std::function<bool(PSInterfaces::Layer* layer)> m_custom_spawn;
		std::function<void(std::shared_ptr<T> entity)> m_spawn_cb;
		std::vector<std::shared_ptr<T>> m_entities;
		float m_interval	  = 100;
		float m_interval_left = 0;
		int m_variation		  = 0;
		int m_limit			  = 10;

		bool m_running = false;
		bool m_render  = false;

		static bool is_suspended_(std::shared_ptr<T> entity)
		{
			if ( std::shared_ptr<PSInterfaces::IEntity> casted = std::dynamic_pointer_cast<PSInterfaces::IEntity>(entity) ) {
				return !casted->is_active();
			}

			return false;
		};

		int count_active_()
		{
			return std::count_if(m_entities.begin(), m_entities.end(), [this](std::shared_ptr<T> entity) { return !is_suspended_(entity); });
		}

		void despawn_if_limit_exceeded_()
		{
			const int a_c = count_active_();
			if ( a_c > m_limit ) {
				const int excessive = a_c - m_limit;

				std::vector<std::shared_ptr<T>> active_entities;
				std::copy_if(m_entities.begin(), m_entities.end(), std::back_inserter(active_entities), [](std::shared_ptr<T> entity) {
					return !is_suspended_(entity);
				});

				for ( int i = a_c; auto& type: active_entities ) {
					if ( i-- <= m_limit )
						break;

					if ( std::shared_ptr<PSInterfaces::IEntity> entity = std::dynamic_pointer_cast<PSInterfaces::IEntity>(type) )
						entity->set_is_active(false);
				}
			}
		}
	};
} // namespace PSCore
