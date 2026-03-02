#pragma once

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace PSCore {

	class Settings
	{
	public:
		Settings(std::string_view filename = "settings.cfg", bool human_readable = true);
		~Settings();

		using setting_val = std::variant<int, float, bool, std::string>;

		std::optional<setting_val> value(std::string_view key);

		void set_value(std::string_view key, const setting_val& value);

		void load();
		void safe();

	private:
		std::string filename_;
		bool human_readable_;

		std::unordered_map<std::string, setting_val> settings_;

		void write_file_();
		void read_file_();
	};

	class SettingsManager
	{
	public:
		SettingsManager()  = default;
		~SettingsManager() = default;

		static SettingsManager* inst()
		{
			static SettingsManager instance;
			return &instance;
		}

		void add_settings(std::string_view name, std::unique_ptr<Settings> new_settings)
		{
			settings.emplace(name, std::move(new_settings));
		}

		std::unique_ptr<Settings>& setting(std::string_view name)
		{
			if ( settings.find(std::string(name)) == settings.end() )
				throw std::out_of_range("Settings with name '" + std::string(name) + "' not found.");

			return settings.at(std::string(name));
		}

		std::unordered_map<std::string, std::unique_ptr<Settings>> settings;
	};
} // namespace PSCore

template<typename T>
T CFG_VALUE(std::string_view key, const T& default_value)
{
	auto instance = PSCore::SettingsManager::inst();
	if ( instance->settings.find("balancing") == instance->settings.end() ) {
		instance->add_settings("balancing", std::make_unique<PSCore::Settings>("balancing_settings.cfg", true));
	}

	auto& bl_s = instance->settings.at("balancing");
	if ( auto val = bl_s->value(key) ) {
		if ( std::holds_alternative<T>(*val) )
			return std::get<T>(*val);
	}

	bl_s->set_value(key, default_value);
	bl_s->safe();
	return default_value;
}
