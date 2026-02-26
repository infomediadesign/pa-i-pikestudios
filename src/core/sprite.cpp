#include <pscore/sprite.h>

using PSCore::sprites::SpriteSheetAnimation;

SpriteSheetAnimation::SpriteSheetAnimation(const Texture2D& texture, const std::vector<SpriteSheetData>& sprite_sheet_data)
{
	m_texture			= texture;
	m_sprite_sheet_data = sprite_sheet_data;

	for ( int i = 0; auto element: m_sprite_sheet_data ) {

		// Sets the m_sprite_time_direction_data Vector based on the m_m_sprite_sheet_data
		if ( m_sprite_time_direction_data.size() <= i ) {
			switch ( element.play_style ) {
				case Forward:
					m_sprite_time_direction_data.push_back({0, 1});
					break;
				case Backward:
					m_sprite_time_direction_data.push_back({0, -1});
					break;
				case PingPong:
					m_sprite_time_direction_data.push_back({0, 1});
					break;
				case KeyFrame:
					m_sprite_time_direction_data.push_back({0, 0});
					break;
			}
		} else {
			switch ( element.play_style ) {
				case Forward:
					m_sprite_time_direction_data.at(i) = {0, 1};
					break;
				case Backward:
					m_sprite_time_direction_data.at(i) = {0, -1};
					break;
				case PingPong:
					m_sprite_time_direction_data.at(i) = {0, 1};
					break;
				case KeyFrame:
					m_sprite_time_direction_data.at(i) = {0, 0};
					break;
			}
		}

		// Calculates the max Value of Frames of the Animations for the m_frame_wight
		if ( element.frames > m_max_frame_in_animation ) {
			m_max_frame_in_animation = element.frames;
		}

		i++;
	}

	m_frame_wight  = static_cast<float>(m_texture.width) / static_cast<float>(m_max_frame_in_animation);
	m_frame_height = static_cast<float>(m_texture.height) / static_cast<float>(m_sprite_sheet_data.size());
}

std::optional<Rectangle> SpriteSheetAnimation::get_source_rectangle(int z_index) const
{
	for ( const auto& element: m_sprite_source_rectangle ) {
		if ( element.z_index == z_index ) {
			return element.rectangle;
		}
	}
	return std::nullopt;
}

void SpriteSheetAnimation::add_animation_at_index(int sprite_sheet_animation_index, int z_index)
{
	int c_sprite_sheet_animation_index = std::clamp(sprite_sheet_animation_index, 0, static_cast<int>(m_sprite_sheet_data.size() - 1));

	// Check if the Animation on the Index exists and if true the Animation is set to the sprite_sheet_animation_index
	for ( auto& element: m_sprite_source_rectangle ) {
		if ( element.z_index == z_index ) {
			if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
				m_sprite_time_direction_data.at(c_sprite_sheet_animation_index).timestamp = 0;
				element.rectangle = {0, static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height, m_frame_wight, m_frame_height};
			}
			return;
		}
	}
	// If the Animation not exists add the Animation to the Vector with the given Values
	if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
		m_sprite_source_rectangle.push_back(
				{{0, static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height, m_frame_wight, m_frame_height}, z_index}
		);
	}
}

void SpriteSheetAnimation::update_animation(float dt)
{
	for ( auto& element: m_sprite_source_rectangle ) {
		SpriteSheetData animation_sprite_sheet_data				= m_sprite_sheet_data.at(element.rectangle.y / m_frame_height);
		SpriteTimeDirectionData animation_sprite_direction_data = m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height);

		// Updates the Animation if the Play style is Forward
		if ( animation_sprite_sheet_data.play_style == Forward ) {
			m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp += dt;
			if ( animation_sprite_direction_data.timestamp >= animation_sprite_sheet_data.play_duration ) {
				m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp = 0;
				play_animation_forward(&element);
			}
		}
		// Updates the Animation if the Play style is Backward
		if ( animation_sprite_sheet_data.play_style == Backward ) {
			m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp += dt;
			if ( animation_sprite_direction_data.timestamp >= animation_sprite_sheet_data.play_duration ) {
				m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp = 0;
				play_animation_backward(&element);
			}
		}
		// Updates the Animation if the Play style is PingPong
		if ( animation_sprite_sheet_data.play_style == PingPong ) {
			m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp += dt;
			if ( animation_sprite_direction_data.timestamp >= animation_sprite_sheet_data.play_duration ) {
				m_sprite_time_direction_data.at(element.rectangle.y / m_frame_height).timestamp = 0;
				play_animation_pingpong(&element);
			}
		}
	}
}

std::optional<int> SpriteSheetAnimation::get_sprite_sheet_animation_index(int z_index)
{
	for ( auto& element: m_sprite_source_rectangle ) {
		if ( element.z_index == z_index ) {
			return element.rectangle.y / m_frame_height;
		}
	}
	return std::nullopt;
}

std::optional<int> SpriteSheetAnimation::get_sprite_sheet_frame_index(int z_index)
{
	for ( auto& element: m_sprite_source_rectangle ) {
		if ( element.z_index == z_index ) {
			return element.rectangle.x / m_frame_wight;
		}
	}
	return std::nullopt;
}

void SpriteSheetAnimation::set_animation_at_index(int sprite_sheet_animation_index, int sprite_sheet_frame_index, int z_index)
{
	int c_sprite_sheet_animation_index = std::clamp(sprite_sheet_animation_index, 0, static_cast<int>(m_sprite_sheet_data.size() - 1));

	for ( auto& element: m_sprite_source_rectangle ) {
		// Checks if the given Values a Valid on the Animation and for Relocation
		if ( element.z_index == z_index ) {
			if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).z_index == z_index ) {
				int c_sprite_sheet_frame_index =
						std::clamp(sprite_sheet_frame_index, 0, m_sprite_sheet_data.at(c_sprite_sheet_animation_index).frames - 1);

				if ( m_sprite_sheet_data.at(c_sprite_sheet_animation_index).frames > c_sprite_sheet_frame_index ) {
					// Sets the Animation to the given Values
					m_sprite_time_direction_data.at(c_sprite_sheet_animation_index).timestamp = 0;
					element.rectangle.y = static_cast<float>(c_sprite_sheet_animation_index) * m_frame_height;
					element.rectangle.x = static_cast<float>(c_sprite_sheet_frame_index) * m_frame_wight;
				}
			}
		}
	}
}

void SpriteSheetAnimation::set_new_animation_framerate(int sprite_sheet_animation_index, float framerate)
{
	int c_sprite_sheet_animation_index = std::clamp(sprite_sheet_animation_index, 0, static_cast<int>(m_sprite_sheet_data.size() - 1));
	m_sprite_sheet_data.at(c_sprite_sheet_animation_index).play_duration = framerate;
}

void SpriteSheetAnimation::play_animation_forward(SpriteSourceRectangle* animation_rectangle)
{
	animation_rectangle->rectangle.x += m_frame_wight;

	// If the current Frame is greater as the max Frame, sets Frame to 0
	if ( animation_rectangle->rectangle.x >=
		 static_cast<float>(m_sprite_sheet_data.at(animation_rectangle->rectangle.y / m_frame_height).frames) * m_frame_wight ) {
		animation_rectangle->rectangle.x = 0;
	}
}

void SpriteSheetAnimation::play_animation_backward(SpriteSourceRectangle* animation_rectangle)
{
	animation_rectangle->rectangle.x -= m_frame_wight;

	// If the current Frame is smaller as 0, sets Frame to the Max Frame
	if ( animation_rectangle->rectangle.x < 0 ) {
		animation_rectangle->rectangle.x =
				static_cast<float>(m_sprite_sheet_data.at(animation_rectangle->rectangle.y / m_frame_height).frames - 1) * m_frame_wight;
	}
}


void SpriteSheetAnimation::play_animation_pingpong(SpriteSourceRectangle* animation_rectangle)
{
	// Updates the Animation Frame by one based on the Direction
	animation_rectangle->rectangle.x +=
			static_cast<float>(m_sprite_time_direction_data.at(animation_rectangle->rectangle.y / m_frame_height).direction) * m_frame_wight;

	// If the current Frame is at the max Frame, sets Play direction to -1
	if ( animation_rectangle->rectangle.x >=
		 static_cast<float>(m_sprite_sheet_data.at(animation_rectangle->rectangle.y / m_frame_height).frames - 1) * m_frame_wight ) {
		m_sprite_time_direction_data.at(animation_rectangle->rectangle.y / m_frame_height).direction = -1;
	}

	// If the current Frame is at 0, sets Play direction to 1
	if ( animation_rectangle->rectangle.x <= 0 ) {
		m_sprite_time_direction_data.at(animation_rectangle->rectangle.y / m_frame_height).direction = 1;
	}
}

using PSCore::sprites::Sprite;

Rectangle Sprite::frame_rect(const Vector2& pos)
{
	Rectangle rect;

	rect.width	= m_s_texture.width / m_s_frame_grid.x;
	rect.height = m_s_texture.height / m_s_frame_grid.y;
	rect.x		= pos.x * rect.width;
	rect.y		= pos.y * rect.height;
	return rect;
}

using PSCore::sprites::SpriteLoader;

SpriteLoader::~SpriteLoader()
{
	for ( auto it = m_texture_cache.begin(); it != m_texture_cache.end(); ) {
		unload(it->first);
		it = m_texture_cache.begin();
	}
}

std::shared_ptr<Sprite>
SpriteLoader::preload(const std::string& ident, const std::string& texture_path, const Vector2& frame_grid, bool replace_if_exists)
{
	if ( m_texture_cache.contains(ident) ) {
		if ( replace_if_exists )
			unload(ident);
		else
			return fetch_sprite(ident);
	}

	PS_LOG(LOG_INFO, TextFormat("Loading texture '%s' (%s) into cache...", texture_path.c_str(), ident.c_str()));
	auto sp			   = std::make_shared<Sprite>();
	sp->m_s_texture	   = LoadTexture(texture_path.data());
	sp->m_s_frame_grid = frame_grid;

	m_texture_cache.insert({ident, std::move(sp)});
	PS_LOG(LOG_INFO, TextFormat("Loaded texture '%s' into cache!", ident.c_str()));
	PS_LOG(LOG_INFO, TextFormat("Texture cache size updated to: %i!", m_texture_cache.size()));

	return fetch_sprite(ident);
}

int SpriteLoader::unload(const std::string& ident)
{
	auto sp = m_texture_cache.at(ident);
	UnloadTexture(sp->m_s_texture);

	return m_texture_cache.erase(ident);
}

std::shared_ptr<Sprite> SpriteLoader::fetch_sprite(const std::string& ident)
{
	try {
		return m_texture_cache.at(ident);
	} catch ( const std::out_of_range& e ) {
		PS_LOG(LOG_ERROR, TextFormat("Could not accsses Sprite: %s", ident.c_str()));
		return std::make_shared<Sprite>();
	}
}
