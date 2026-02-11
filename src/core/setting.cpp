#include <iostream>
#include <optional>
#include <pscore/application.h>
#include <pscore/settings.h>
#include <raylib.h>
#include <string>

#include <pscore/filemanager.h>

using PSCore::Settings;

static const std::string BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"abcdefghijklmnopqrstuvwxyz"
										"0123456789+/";

std::string base64_encode(const std::string& in)
{
	std::string out;
	int val = 0, valb = -6;
	for ( unsigned char c: in ) {
		val = (val << 8) + c;
		valb += 8;
		while ( valb >= 0 ) {
			out.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if ( valb > -6 )
		out.push_back(BASE64_CHARS[((val << 8) >> valb) & 0x3F]);
	while ( out.size() % 4 )
		out.push_back('=');
	return out;
}

std::string base64_decode(const std::string& in)
{
	std::string out;
	std::vector<int> T(256, -1);
	for ( int i = 0; i < 64; i++ )
		T[BASE64_CHARS[i]] = i;

	int val = 0, valb = -8;
	for ( unsigned char c: in ) {
		if ( T[c] == -1 )
			break;
		val = (val << 6) + T[c];
		valb += 6;
		if ( valb >= 0 ) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}

// Load and save the unordered map of settings to a file, so that they persist between sessions
void Settings::write_file_()
{
	Filemanager file(filename_);
	std::string data;
	for ( const auto& [key, value]: settings_ ) {
		if ( auto i = std::get_if<int>(&value) )
			data += key + "=" + std::to_string(*i) + "\n";
		else if ( auto b = std::get_if<bool>(&value) )
			data += key + "=" + (*b ? "true" : "false") + "\n";
		else if ( auto f = std::get_if<float>(&value) )
			data += key + "=" + std::to_string(*f) + "\n";
		else if ( auto s = std::get_if<std::string>(&value) )
			data += key + "=" + *s + "\n";
	}

	if ( !human_readable_ )
		data = base64_encode(data);

	file.write(data);
}

void Settings::read_file_()
{
	Filemanager file(filename_);
	std::string data = file.load();

	if ( !human_readable_ )
		data = base64_decode(data);

	size_t pos = 0;
	while ( (pos = data.find("\n")) != std::string::npos ) {
		std::string line = data.substr(0, pos);
		data.erase(0, pos + 1);
		size_t eq_pos = line.find("=");

		if ( eq_pos == std::string::npos )
			continue;

		std::string key	  = line.substr(0, eq_pos);
		std::string value = line.substr(eq_pos + 1);

		if ( value == "true" )
			settings_[key] = true;
		else if ( value == "false" )
			settings_[key] = false;
		else if ( value.find(".") != std::string::npos )
			settings_[key] = std::stof(value);
		else
			settings_[key] = std::stoi(value);
	}
}

Settings::Settings(std::string_view filename, bool human_readable) : filename_(filename), human_readable_(human_readable)
{
	load();
	std::string log{"Settings loaded from file: "};
	log += filename_.c_str();
	PS_LOG(LOG_INFO, log);
};

Settings::~Settings()
{
	safe();
};

void Settings::set_value(std::string_view key, const setting_val& value)
{
	settings_.insert({std::string{key}, value});
}

std::optional<Settings::setting_val> Settings::value(std::string_view key)
{
	if ( settings_.find(std::string{key}) == settings_.end() )
		return std::nullopt;

	return settings_.at(std::string{key});
};

void Settings::load()
{
	read_file_();
}

void Settings::safe()
{
	write_file_();
}
