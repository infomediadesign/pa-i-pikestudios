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

		enum PlayStyle { Forward, PingPong, KeyFrame };

		struct SpriteSheetData
		{
			int frames;
			float play_duration;
			PlayStyle play_style;
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
			/**
			 * @brief The Init Function for the Animation Controller
			 * @details The Sprite Sheet Data is a description of every Animation in the Sprite Sheet
			 * {Frames in this Animation, Play duration of every Frame, The Play style of the Animation, The Z Index of the Animation}, {...}, ...
			 * @param texture A Reference to the Sprite Sheet Texture
			 * @param sprite_sheet_data A Reference to the Data of the Sprite Sheet
			 */
			SpriteSheetAnimation(const Texture2D& texture, const std::vector<SpriteSheetData>& sprite_sheet_data)
			{
				m_texture			= texture;
				m_sprite_sheet_data = sprite_sheet_data;

				for ( int i = 0; auto element: m_sprite_sheet_data ) {

					//Sets the m_sprite_time_direction_data Vector based on the m_m_sprite_sheet_data
					if ( m_sprite_time_direction_data.size() <= i ) {
						if ( element.play_style == KeyFrame ) {
							m_sprite_time_direction_data.push_back({0, 0});
						} else {
							m_sprite_time_direction_data.push_back({0, 1});
						}
					} else {
						if ( element.play_style == KeyFrame ) {
							m_sprite_time_direction_data.at(i) = {0, 0};
						} else {
							m_sprite_time_direction_data.at(i) = {0, 1};
						}
					}

					//Calculates the max Value of Frames of the Animations for the m_frame_wight
					if ( element.frames > m_max_frame_in_animation ) {
						m_max_frame_in_animation = element.frames;
					}

					i++;
				}

				m_frame_wight  = static_cast<float>(m_texture.width) / static_cast<float>(m_max_frame_in_animation);
				m_frame_height = static_cast<float>(m_texture.height) / static_cast<float>(m_sprite_sheet_data.size());
			}

			/**
			 * @brief Returns the Source Rectangle of the Animation aat the given Index
			 * @param z_index The Z Index of the Animation
			 * @return The Source Rectangle of the Animation at that Index
			 */
			Rectangle get_source_rectangle(int z_index) const
			{
				for ( const auto& element: m_sprite_source_rectangle ) {
					if ( element.z_index == z_index ) {
						return element.rectangle;
					}
				}
				return {};
			}

			/**
			 * @brief Add an Animation to the Animation Controller at the given Animation Index and Z Index
			 * @details If the animation on the Z Index already exists the Animation Jumps to the first Frame of the Animation on the given Animation
			 * Index (Do not on Purpose)
			 * @param sprite_sheet_animation_index The Index of the Animation to add to the Animation
			 * @param z_index The Z Index of the Animation
			 */
			void add_animation_at_index(int sprite_sheet_animation_index, int z_index)
			{
				int c_sprite_sheet_animation_index = std::clamp(sprite_sheet_animation_index, 0, static_cast<int>(m_sprite_sheet_data.size() - 1));

				//Check if the Animation on the Index exists and if true the Animation is set to the sprite_sheet_animation_index
				for ( auto& element: m_sprite_source_rectangle ) {
					if ( element.z_index == z_index ) {
						if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
							m_sprite_time_direction_data.at(c_sprite_sheet_animation_index).timestamp = 0;
							element.rectangle														  = {
									0, static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height, m_frame_wight, m_frame_height
							};
						}
						return;
					}
				}
				//If the Animation not exists add the Animation to the Vector with the given Values
				if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
					m_sprite_source_rectangle.push_back(
							{{0, static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height, m_frame_wight, m_frame_height}, z_index}
					);
				}
			}

			/**
			 * @brief Updates the added Animation based on the given Play style
			 * @param dt Delta Time
			 */
			void update_animation(float dt)
			{
				for ( auto& element: m_sprite_source_rectangle ) {
					SpriteSheetData animation_sprite_sheet_data				= m_sprite_sheet_data.at(element.rectangle.y / m_frame_height);
					SpriteTimeDirectionData animation_sprite_direction_data = m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height);

					//Updates the Animation if the Play style is Forward
					if ( animation_sprite_sheet_data.play_style == Forward ) {
						m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp += dt;
						if ( animation_sprite_direction_data.timestamp >= animation_sprite_sheet_data.play_duration ) {
							m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp = 0;
							play_animation_forward(element);
						}
					}
					//Updates the Animation if the Play style is PingPong
					if ( animation_sprite_sheet_data.play_style == PingPong ) {
						m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp += dt;
						if ( animation_sprite_direction_data.timestamp >= animation_sprite_sheet_data.play_duration ) {
							m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp = 0;
							play_animation_pingpong(element);
						}
					}
				}
			}

			/**
			 * @brief Calculates the Animation Index of the Animation based on the Z Index
			 * @param z_index The Z Index of the Animation
			 * @return The Animation Index of the Sprite Sheet
			 */
			int get_sprite_sheet_animation_index(int z_index)
			{
				for ( auto& element: m_sprite_source_rectangle ) {
					if ( element.z_index == z_index ) {
						return element.rectangle.y / m_frame_height;
					}
				}
				return {};
			}

			/**
			 * @brief Sets an Animation of an Z Index to another Animation with the given Frame in the Animation
			 * @param sprite_sheet_animation_index The Index of the Animation in the Sprite Sheet
			 * @param sprite_sheet_frame_index The Index of the Frame in the Animation
			 * @param z_index The Z Index of the Animation
			 */
			void set_animation_at_index(int sprite_sheet_animation_index, int sprite_sheet_frame_index, int z_index)
			{
				int c_sprite_sheet_animation_index = std::clamp(sprite_sheet_animation_index, 0, static_cast<int>(m_sprite_sheet_data.size() - 1));

				for ( auto& element: m_sprite_source_rectangle ) {
					//Checks if the given Values a Valid on the Animation and for Relocation
					if ( element.z_index == z_index ) {
						if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
							int c_sprite_sheet_frame_index =
									std::clamp(sprite_sheet_frame_index, 0, m_sprite_sheet_data.at(element.rectangle.y / m_frame_height).frames - 1);

							if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).frames > c_sprite_sheet_frame_index ) {
								//Sets the Animation to the given Values
								m_sprite_time_direction_data.at(c_sprite_sheet_animation_index).timestamp = 0;
								element.rectangle.y = static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height;
								element.rectangle.x = static_cast<float>(c_sprite_sheet_frame_index) * m_frame_wight;
							}
						}
					}
				}
			}

		private:
			/**
			 * @brief Set the Animation one Frame forward in a Forward loop
			 * @param animation_rectangle The Reference to an Animation
			 */
			void play_animation_forward(SpriteSourceRectangle& animation_rectangle)
			{
				animation_rectangle.rectangle.x += m_frame_wight;

				//If the current Frame is greater as the max Frame, sets Frame to 0
				if ( animation_rectangle.rectangle.x >=
					 static_cast<float>(m_sprite_sheet_data.at(animation_rectangle.rectangle.y / m_frame_height).frames) * m_frame_wight ) {
					animation_rectangle.rectangle.x = 0;
				}
			}

			/**
			 * @Set the Animation one Frame forward or backward in a PingPong loop
			 * @param animation_rectangle The Reference to an Animation
			 */
			void play_animation_pingpong(SpriteSourceRectangle& animation_rectangle)
			{
				//Updates the Animation Frame by one based on the Direction
				animation_rectangle.rectangle.x +=
						static_cast<float>(m_sprite_time_direction_data.at(animation_rectangle.rectangle.y / m_frame_height).direction) *
						m_frame_wight;

				//If the current Frame is at the max Frame, sets Play direction to -1
				if ( animation_rectangle.rectangle.x >=
					 static_cast<float>(m_sprite_sheet_data.at(animation_rectangle.rectangle.y / m_frame_height).frames - 1) * m_frame_wight ) {
					m_sprite_time_direction_data.at(animation_rectangle.rectangle.y / m_frame_height).direction = -1;
				}

				//If the current Frame is at 0, sets Play direction to 1
				if ( animation_rectangle.rectangle.x <= 0 ) {
					m_sprite_time_direction_data.at(animation_rectangle.rectangle.y / m_frame_height).direction = 1;
				}
			}

			Texture2D m_texture			 = {};
			float m_frame_wight			 = 0;
			float m_frame_height		 = 0;
			int m_max_frame_in_animation = 0;
			std::vector<SpriteSheetData> m_sprite_sheet_data;
			std::vector<SpriteTimeDirectionData> m_sprite_time_direction_data;
			std::vector<SpriteSourceRectangle> m_sprite_source_rectangle = {};
		};

		struct Sprite
		{
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
