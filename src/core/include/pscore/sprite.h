#pragma once

#include <memory>
#include <pscore/application.h>
#include <raylib.h>
#include <stdexcept>
#include <unordered_map>
#include <valarray>

#define PRELOAD_TEXTURE(ident, path) gApp()->sprite_loader()->preload(ident, path)
#define FETCH_SPRITE(ident) gApp()->sprite_loader()->fetch_sprite(ident)
#define FETCH_SPRITE_TEXTURE(ident) gApp()->sprite_loader()->fetch_sprite(ident)->m_s_sprite

namespace PSCore {

	namespace sprites {

		struct Sprite
		{
			std::valarray<int> m_s_sprite_sheet;

			float m_s_animation_speed = 1;
			float m_s_frame_counter	  = 0;
			float m_s_animation_count = 0;
			float m_s_animation_frame = 0;

			Texture2D m_s_sprite;
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

			void preload(const std::string& ident, const std::string& texture_path)
			{
				if ( m_texture_cache.contains(ident) )
					return;

				auto sp		   = std::make_shared<Sprite>();
				sp->m_s_sprite = LoadTexture(texture_path.data());

				m_texture_cache.insert({ident, std::move(sp)});
				PS_LOG(LOG_INFO, TextFormat("Loaded texture '%s' into cache!", ident.c_str()));
				PS_LOG(LOG_INFO, TextFormat("Texture cache size updated to: %i!", m_texture_cache.size()));
			}

			int unload(const std::string& ident)
			{
				auto sp = m_texture_cache.at(ident);
				UnloadTexture(sp->m_s_sprite);

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
