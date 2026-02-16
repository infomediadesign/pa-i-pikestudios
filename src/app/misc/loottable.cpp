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

void LootTable::add_loot_table(int index, int pull_chance, std::vector<int>& chances)
{
	for ( auto element: m_indices ) {
		if ( element.index == index ) {
			return;
		}
	}

	int chances_sum = 0;
	for ( int i = 0; i < chances.size(); i++ ) {
		chances_sum += chances.at(i);

		if ( i == chances.size() - 1 ) {
			if ( chances_sum != 100 ) {
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

	if ( indices_count > m_indices.size() ) {
		count = static_cast<int>(m_indices.size());
	}

	for ( auto index: indices ) {
		largest_random_chance += index.pull_chance;
	}

	while ( m_values.size() < count ) {
		m_values.push_back({-1, -1, -1});
	}

	for ( int i = 0; i < count; i++ ) {
		int random_index = PSUtils::gen_rand(0, largest_random_chance - 1);

		int previous_pull_chance_sum = 0;

		for ( int j = 0; auto& index: indices ) {
			if ( index.index != -1 ) {
				if ( random_index < index.pull_chance + previous_pull_chance_sum ) {
					m_values.at(i).index = m_indices.at(j).index;
					m_values.at(i).value = PSUtils::gen_rand(0, 99);

					largest_random_chance -= index.pull_chance;

					index.index = -1;

					break;
				}
				previous_pull_chance_sum += index.pull_chance;
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

				if ( curve_value <= static_cast<float>(chance.chance) ) {
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
		for ( int i = 0; auto index: m_indices ) {
			if ( value.index == index.index ) {
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
					if ( static_cast<float>(value.value) > curve_y ) {
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
