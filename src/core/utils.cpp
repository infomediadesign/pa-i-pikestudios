#include <PSCore/utils.h>

#include <random>

int PSUtils::gen_rand(const int min, const int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(min, max);

	return distr(gen);
}
