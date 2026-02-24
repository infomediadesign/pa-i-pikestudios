#include "loottable.h"

#include <random>

#include <pscore/utils.h>

#ifndef SIGMA
#define SIGMA 4
#endif

#ifndef ACCURACY
#define ACCURACY 10
#endif

#ifndef SQRT2HALF
#define SQRT2HALF 0.7071067811865f
#endif

void LootTable::add_loot_table(int index, int pull_chance, std::vector<float>& chances)
{
	for ( auto element: m_indices ) {
		if ( element.index == index ) {
			return;
		}
	}

	float chances_sum = 0;
	for ( int i = 0; i < chances.size(); i++ ) {
		chances_sum += chances.at(i);

		if ( i == chances.size() - 1 ) {
			if ( abs(chances_sum - 100) > 0.001 ) {
				return;
			}
		}
	}

	if ( pull_chance <= 0 ) {
		return;
	}

	int location = static_cast<int>(m_chances.size());
	m_indices.push_back({index, pull_chance, location});

	for ( int i = 0; auto chance: chances ) {
		if ( i == 0 ) {
			m_chances.push_back({chance, 0});
		} else {
			m_chances.push_back({chance + m_chances.at(location + i - 1).chance, 0});
		}
		i++;
	}

	calculate_curve_boundary();
}

void LootTable::set_expected_value(float expected_value)
{
	m_expected_value = std::clamp(expected_value, -1.0f, 1.0f);
}

void LootTable::set_pull_chance(int index, int pull_chance)
{
	for ( auto& element: m_indices ) {
		if ( element.index == index ) {
			element.pull_chance = pull_chance;
		}
	}
}

std::vector<LootTableValue> LootTable::loot_table_values(int count)
{
	random_values_from_loot_table(count);

	calculate_rarity();

	return m_values;
}

void LootTable::random_values_from_loot_table(int indices_count)
{
	int count							  = indices_count;
	int largest_random_chance			  = 0;
	std::vector<LootTableIndices> indices = m_indices;

	int valid_pull_chance_count = 0;
	for ( auto& element: indices ) {
		largest_random_chance += element.pull_chance;

		if ( element.pull_chance == 0 ) {
			element.index = -1;
		} else {
			valid_pull_chance_count++;
		}
	}

	if ( indices_count > valid_pull_chance_count ) {
		count = valid_pull_chance_count;
	}

	while ( m_values.size() < count ) {
		m_values.push_back({-1, -1, -1});
	}

	for ( int i = 0; i < count; i++ ) {
		int random_index = PSUtils::gen_rand(0, largest_random_chance - 1);

		int previous_pull_chance_sum = 0;

		for ( int j = 0; auto& element: indices ) {
			if ( element.index != -1 ) {
				if ( random_index < element.pull_chance + previous_pull_chance_sum ) {
					m_values.at(i).index = m_indices.at(j).index;
					m_values.at(i).value = static_cast<float>(PSUtils::gen_rand(0, 9999)) / 100;

					largest_random_chance -= element.pull_chance;

					element.index = -1;

					break;
				}
				previous_pull_chance_sum += element.pull_chance;
			}
			j++;
		}
	}
}

void LootTable::calculate_curve_boundary()
{
	float delta_sigma = 2 * SIGMA;

	for ( auto& chance: m_chances ) {
		if ( chance.curve_boundary == 0 ) {
			int divisor	   = 2;
			float boundary = -SIGMA;
			for ( int i = 0; i < ACCURACY; i++ ) {
				float curve_value =
						static_cast<float>(0.5 * std::erff(SQRT2HALF * ((delta_sigma / static_cast<float>(divisor)) + boundary)) + 0.5) * 100;

				if ( curve_value <= chance.chance ) {
					boundary += delta_sigma / static_cast<float>(divisor);
				}

				divisor *= 2;
			}
			chance.curve_boundary = boundary;
		}
	}
}

void LootTable::calculate_rarity()
{
	for ( auto& value: m_values ) {
		for ( int i = 0; auto element: m_indices ) {
			if ( value.index == element.index ) {
				value.rarity = 0;

				int length;
				if ( m_indices.size() - 1 == i ) {
					length = static_cast<int>(m_chances.size()) - m_indices.at(i).location;
				} else {
					length = m_indices.at(i + 1).location - m_indices.at(i).location;
				}

				for ( int j = 0; j < length; j++ ) {
					float curve_x = (m_chances.at(j + m_indices.at(i).location).curve_boundary - m_expected_value);
					float curve_y = (0.5f * std::erff(SQRT2HALF * curve_x) + 0.5f) * 100;
					if ( value.value > curve_y ) {
						value.rarity = std::min(j + 1, length - 1);
					} else {
						break;
					}
				}
			}
			i++;
		}
	}
}
