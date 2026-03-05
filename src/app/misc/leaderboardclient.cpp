#include <misc/leaderboardclient.h>

#include <httplib.h>
#include <nlohmann/json.hpp>

std::unique_ptr<httplib::Client> makeClient_(const std::string& host)
{
	return std::make_unique<httplib::Client>(host);
}

bool LeaderboardClient::postScore(const std::string& name, int score, int sailColor, std::string* errOut)
{
	using nlohmann::json;

	if ( name.empty() || name.size() > 15 ) {
		if ( errOut )
			*errOut = "Invalid name length";
		return false;
	}
	if ( score < 0 ) {
		if ( errOut )
			*errOut = "Invalid score";
		return false;
	}

	json body = {
			{"name", name},
			{"score", score},
			{"sail_color", sailColor},
	};

	auto client = makeClient_(serverHost_);
	client->set_connection_timeout(3, 0);
	client->set_read_timeout(5, 0);
	client->set_write_timeout(5, 0);

	auto res = client->Post("/api/v1/scores", body.dump(), "application/json");
	if ( !res ) {
		if ( errOut )
			*errOut = "HTTP request failed (no response)";
		return false;
	}
	if ( res->status != 200 ) {
		if ( errOut )
			*errOut = "HTTP error status: " + std::to_string(res->status) + " body: " + res->body;
		return false;
	}

	// optional: parse {"ok":true}
	try {
		auto j = json::parse(res->body);
		if ( j.contains("ok") && j["ok"].is_boolean() && j["ok"].get<bool>() == true )
			return true;
	} catch ( ... ) {
		// ignore parse failure; treat 200 as success
		return true;
	}

	return true;
}
std::optional<std::vector<LeaderboardEntry>> LeaderboardClient::getTopScores(int limit, std::string* errOut)
{
	using nlohmann::json;

	if ( limit < 1 )
		limit = 1;
	if ( limit > 100 )
		limit = 100;

	auto client = makeClient_(serverHost_);
	client->set_connection_timeout(3, 0);
	client->set_read_timeout(5, 0);

	std::string path = "/api/v1/scores?limit=" + std::to_string(limit);
	auto res		 = client->Get(path.c_str());
	if ( !res ) {
		if ( errOut )
			*errOut = "HTTP request failed (no response)";
		return std::nullopt;
	}
	if ( res->status != 200 ) {
		if ( errOut )
			*errOut = "HTTP error status: " + std::to_string(res->status) + " body: " + res->body;
		return std::nullopt;
	}

	std::vector<LeaderboardEntry> out;
	try {
		auto j = json::parse(res->body);
		if ( !j.contains("scores") || !j["scores"].is_array() ) {
			if ( errOut )
				*errOut = "Invalid JSON: missing scores[]";
			return std::nullopt;
		}

		for ( auto& item: j["scores"] ) {
			LeaderboardEntry e;
			e.name		 = item.value("name", "");
			e.score		 = item.value("score", 0);
			e.sail_color = item.value("sail_color", 0);
			out.push_back(std::move(e));
		}
	} catch ( const std::exception& ex ) {
		if ( errOut )
			*errOut = std::string("JSON parse error: ") + ex.what();
		return std::nullopt;
	}

	return out;
}


LeaderboardClient::LeaderboardClient(std::string serverHost) : serverHost_(std::move(serverHost))
{
}
