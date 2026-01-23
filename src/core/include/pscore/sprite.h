#pragma once

#include <memory>
#include <pscore/application.h>
#include <raylib.h>
#include <stdexcept>
#include <unordered_map>
#include <valarray>

#define PRELOAD_TEXTURE(ident, path, frame_grid) gApp()->sprite_loader()->preload(ident, path, frame_grid)
#define FETCH_SPRITE(ident) gApp()->sprite_loader()->fetch_sprite(ident)
#define FETCH_SPRITE_TEXTURE(ident) gApp()->sprite_loader()->fetch_sprite(ident)->m_s_texture

namespace PSCore {

	namespace sprites {

		enum PlayDirection {Forward, PingPong, KeyFrame};

		struct SpriteSheetData
		{
			int frames;
			float play_duration;
			PlayDirection play_direction;
			int z_index;
		};

		struct SpriteTimeDirectionData
		{
			float timestamp;
			int direction;
		};

		struct SpriteSourceRectangle
		{
			Rectangle rectangle;
			int z_index;
		};

		class SpriteSheetAnimation
		{
		public:
			explicit SpriteSheetAnimation(Texture2D texture, const std::vector<SpriteSheetData>& sprite_sheet_data)
			{
				m_texture = texture;
				m_sprite_sheet_data = sprite_sheet_data;

				for (int i = 0; auto element : m_sprite_sheet_data) {

					if ( m_sprite_time_direction_data.size() <= i ) {
						if ( element.play_direction == KeyFrame ) {
							m_sprite_time_direction_data.push_back({0,0});
						}
						else {
							m_sprite_time_direction_data.push_back({0,1});
						}
					}
					else {
						if ( element.play_direction == KeyFrame ) {
							m_sprite_time_direction_data.at(i) = {0,0};
						}
						else {
							m_sprite_time_direction_data.at(i) = {0,1};
						}
					}

					if ( element.frames > m_max_frame_in_animation ) {
						m_max_frame_in_animation = element.frames;
					}

					i++;
				}

				m_frame_wight = static_cast<float>(m_texture.width) / static_cast<float>(m_max_frame_in_animation);
				m_frame_height = static_cast<float>(m_texture.height) / static_cast<float>(m_sprite_sheet_data.size());
			}

			Rectangle get_source_rectangle(int z_index)
			{
				for ( auto element : m_sprite_source_rectangle ) {
					if ( element.z_index == z_index ) {
						return element.rectangle;
					}
				}
				return {};
			}

			void add_animation_at_index(int sprite_sheet_animation_index, int z_index)
			{
				int c_sprite_sheet_animation_index = std::clamp(sprite_sheet_animation_index, 0, static_cast<int>(m_sprite_sheet_data.size() - 1));

				for ( auto element : m_sprite_source_rectangle ) {
					if ( element.z_index == z_index ) {
						if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
							m_sprite_time_direction_data.at(c_sprite_sheet_animation_index).timestamp = 0;
							element.rectangle = {0, static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height, m_frame_wight, m_frame_height};
						}
						return;
					}
				}
				if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
					m_sprite_source_rectangle.push_back({{0, static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height, m_frame_wight, m_frame_height}, z_index});
				}
			}

			void update_animation(float dt)
			{
				for ( auto element : m_sprite_source_rectangle ) {
					SpriteSheetData animation_sprite_sheet_data = m_sprite_sheet_data.at(element.rectangle.y / m_frame_height);
					SpriteTimeDirectionData animation_sprite_direction_data = m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height);

					if ( animation_sprite_sheet_data.play_direction == Forward ) {
						m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp += dt;
						if ( animation_sprite_direction_data.timestamp >= animation_sprite_sheet_data.play_duration ) {
							m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp = 0;
							play_animation_forward(element);
						}
					}
					if ( animation_sprite_sheet_data.play_direction == PingPong ) {
						m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp += dt;
						if ( animation_sprite_direction_data.timestamp >= animation_sprite_sheet_data.play_duration ) {
							m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp = 0;
							play_animation_pingpong(element);
						}
					}
				}
			}

			void set_animation_at_index(int sprite_sheet_animation_index, int sprite_sheet_frame_index, int z_index)
			{
				int c_sprite_sheet_animation_index = std::clamp(sprite_sheet_animation_index, 0, static_cast<int>(m_sprite_sheet_data.size() - 1));

				for ( auto element : m_sprite_source_rectangle ) {
					if ( element.z_index == z_index ) {
						if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
							int c_sprite_sheet_frame_index = std::clamp(sprite_sheet_frame_index, 0, m_sprite_sheet_data.at(element.rectangle.y / m_frame_height).frames - 1);

							if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).frames > c_sprite_sheet_frame_index ) {
								m_sprite_time_direction_data.at(c_sprite_sheet_animation_index).timestamp = 0;
								element.rectangle.y = static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height;
								element.rectangle.x = static_cast<float>(c_sprite_sheet_frame_index) * m_frame_wight;
								return;
							}
						}
					}
				}
			}

		private:
			void play_animation_forward(SpriteSourceRectangle& animation_rectangle)
			{
				animation_rectangle.rectangle.x += m_frame_wight;
				if ( animation_rectangle.rectangle.x >= static_cast<float>(m_sprite_sheet_data.at(animation_rectangle.rectangle.y / m_frame_height).frames) * m_frame_wight) {
					animation_rectangle.rectangle.x = 0;
				}
			}

			void play_animation_pingpong(SpriteSourceRectangle& animation_rectangle)
			{
				animation_rectangle.rectangle.x += static_cast<float>(m_sprite_time_direction_data.at(animation_rectangle.rectangle.y / m_frame_height).direction) * m_frame_wight;
				if ( animation_rectangle.rectangle.x >= static_cast<float>(m_sprite_sheet_data.at(animation_rectangle.rectangle.y / m_frame_height).frames) * m_frame_wight) {
					m_sprite_time_direction_data.at(animation_rectangle.rectangle.y / m_frame_height).direction = -1;
				}
				if ( animation_rectangle.rectangle.x <= 0) {
					m_sprite_time_direction_data.at(animation_rectangle.rectangle.y / m_frame_height).direction = 1;
				}
			}

			Texture2D m_texture = {};
			float m_frame_wight = 0;
			float m_frame_height = 0;
			int m_max_frame_in_animation = 0;
			std::vector<SpriteSheetData> m_sprite_sheet_data;
			std::vector<SpriteTimeDirectionData> m_sprite_time_direction_data;
			std::vector<SpriteSourceRectangle> m_sprite_source_rectangle = {};
		};

		struct Sprite
		{
			std::valarray<int> m_s_sprite_sheet;

			float m_s_animation_speed = 1;
			float m_s_frame_counter	  = 0;
			float m_s_animation_count = 0;
			float m_s_animation_frame = 0;

			Texture2D m_s_texture;
			Vector2 m_s_frame_grid;

			Rectangle frame_rect(const Vector2& pos)
			{
				Rectangle rect;
				
				rect.width	= m_s_texture.width / m_s_frame_grid.x;
				rect.height = m_s_texture.height / m_s_frame_grid.y;
				rect.x		= pos.x * rect.width;
				rect.y		= pos.y * rect.height;
				return rect;
			};
		};

		class SpriteLoader
		{
		public:
			SpriteLoader() {};
			~SpriteLoader()
			{
				for ( auto it = m_texture_cache.begin(); it != m_texture_cache.end(); ) {
					unload(it->first);
					it = m_texture_cache.begin();
				}
			}

			std::shared_ptr<Sprite> preload(const std::string& ident, const std::string& texture_path, const Vector2& frame_grid)
			{
				if ( m_texture_cache.contains(ident) )
					return fetch_sprite(ident);

				auto sp			   = std::make_shared<Sprite>();
				sp->m_s_texture	   = LoadTexture(texture_path.data());
				sp->m_s_frame_grid = frame_grid;

				m_texture_cache.insert({ident, std::move(sp)});
				PS_LOG(LOG_INFO, TextFormat("Loaded texture '%s' into cache!", ident.c_str()));
				PS_LOG(LOG_INFO, TextFormat("Texture cache size updated to: %i!", m_texture_cache.size()));
				
				return fetch_sprite(ident);
			}

			int unload(const std::string& ident)
			{
				auto sp = m_texture_cache.at(ident);
				UnloadTexture(sp->m_s_texture);

				return m_texture_cache.erase(ident);
			}

			std::shared_ptr<Sprite> fetch_sprite(const std::string& ident)
			{
				try {
					return m_texture_cache.at(ident);
				} catch ( const std::out_of_range& e ) {
					PS_LOG(LOG_ERROR, TextFormat("Could not accsses Sprite: %s", ident.c_str()));
					return std::make_shared<Sprite>();
				}
			}

		private:
			std::unordered_map<std::string, std::shared_ptr<Sprite>> m_texture_cache;
		};
	} // namespace sprites
} // namespace PSCore
