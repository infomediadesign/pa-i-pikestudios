#pragma once

#include <memory>
#include <optional>
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

		std::unique_ptr<Settings> balancing_settings = std::make_unique<Settings>("balancing_settings.cfg", true);
	};
} // namespace PSCore

template<typename T>
T CFG_VALUE(std::string_view key, const T& default_value)
{
	auto& bl_s = PSCore::SettingsManager::inst()->balancing_settings;
	if ( auto val = bl_s->value(key) ) {
		if ( std::holds_alternative<T>(*val) )
			return std::get<T>(*val);
	}

	bl_s->set_value(key, default_value);
	bl_s->safe();
	return default_value;
}
