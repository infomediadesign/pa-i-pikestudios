#pragma once

#include <optional>
#include <string>
#include <vector>

struct LeaderboardEntry
{
	std::string name;
	int score	   = 0;
	int sail_color = 0;
};

class LeaderboardClient
{
public:
	explicit LeaderboardClient(std::string serverHost);

	// Returns true on success
	bool postScore(const std::string& name, int score, int sailColor, std::string* errOut = nullptr);

	std::optional<std::vector<LeaderboardEntry>> getTopScores(int limit = 10, std::string* errOut = nullptr);

private:
	std::string serverHost_;
};
