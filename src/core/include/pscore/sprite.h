#pragma once

#include <memory>
#include <pscore/application.h>
#include <raylib.h>
#include <stdexcept>
#include <unordered_map>

#define PRELOAD_TEXTURE(ident, path, frame_grid) gApp()->sprite_loader()->preload(ident, path, frame_grid)
#define FETCH_SPRITE(ident) gApp()->sprite_loader()->fetch_sprite(ident)
#define FETCH_SPRITE_TEXTURE(ident) gApp()->sprite_loader()->fetch_sprite(ident)->m_s_texture

namespace PSCore {

	namespace sprites {

		enum PlayStyle { Forward, Backward, PingPong, KeyFrame };

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
			SpriteSheetAnimation() = default;

			/**
			 * @brief The Init Function for the Animation Controller
			 * @details The Sprite Sheet Data is a description of every Animation in the Sprite Sheet
			 * {Frames in this Animation, Play duration of every Frame, The Play style of the Animation, The Z Index of the Animation}, {...}, ...
			 * @param texture A Reference to the Sprite Sheet Texture
			 * @param sprite_sheet_data A Reference to the Data of the Sprite Sheet
			 */
			SpriteSheetAnimation(const Texture2D& texture, const std::vector<SpriteSheetData>& sprite_sheet_data);

			/**
			 * @brief Returns the Source Rectangle of the Animation aat the given Index
			 * @param z_index The Z Index of the Animation
			 * @return The Source Rectangle of the Animation at that Index
			 */
			std::optional<Rectangle> get_source_rectangle(int z_index) const;

			/**
			 * @brief Add an Animation to the Animation Controller at the given Animation Index and Z Index
			 * @details If the animation on the Z Index already exists the Animation Jumps to the first Frame of the Animation on the given Animation
			 * Index (Do not on Purpose)
			 * @param sprite_sheet_animation_index The Index of the Animation to add to the Animation
			 * @param z_index The Z Index of the Animation
			 */
			void add_animation_at_index(int sprite_sheet_animation_index, int z_index);

			/**
			 * @brief Updates the added Animation based on the given Play style
			 * @param dt Delta Time
			 */
			void update_animation(float dt);

			/**
			 * @brief Calculates the Animation Index of the Animation based on the Z Index
			 * @param z_index The Z Index of the Animation
			 * @return The Animation Index of the Sprite Sheet
			 */
			std::optional<int> get_sprite_sheet_animation_index(int z_index);

			/**
			 * @brief Calculates the Frame Index of the Animation based on the Z Index
			 * @param z_index The Z Index of the Animation
			 * @return The Frame Index of the Sprite Sheet
			 */
			std::optional<int> get_sprite_sheet_frame_index(int z_index);

			/**
			 * @brief Sets an Animation of an Z Index to another Animation with the given Frame in the Animation
			 * @param sprite_sheet_animation_index The Index of the Animation in the Sprite Sheet
			 * @param sprite_sheet_frame_index The Index of the Frame in the Animation
			 * @param z_index The Z Index of the Animation
			 */
			void set_animation_at_index(int sprite_sheet_animation_index, int sprite_sheet_frame_index, int z_index);

		private:
			/**
			 * @brief Set the Animation one Frame forward in a Forward loop
			 * @param animation_rectangle The Reference to an Animation
			 */
			void play_animation_forward(SpriteSourceRectangle* animation_rectangle);

			/**
			 * @brief Set the Animation one Frame backward in a Backward loop
			 * @param animation_rectangle The Reference to an Animation
			 */
			void play_animation_backward(SpriteSourceRectangle* animation_rectangle);

			/**
			 * @Set the Animation one Frame forward or backward in a PingPong loop
			 * @param animation_rectangle The Reference to an Animation
			 */
			void play_animation_pingpong(SpriteSourceRectangle* animation_rectangle);

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

			Rectangle frame_rect(const Vector2& pos);
		};

		class SpriteLoader
		{
		public:
			SpriteLoader() {};
			~SpriteLoader();

			std::shared_ptr<Sprite> preload(const std::string& ident, const std::string& texture_path, const Vector2& frame_grid);

			int unload(const std::string& ident);

			std::shared_ptr<Sprite> fetch_sprite(const std::string& ident);

		private:
			std::unordered_map<std::string, std::shared_ptr<Sprite>> m_texture_cache;
		};
	} // namespace sprites
} // namespace PSCore
