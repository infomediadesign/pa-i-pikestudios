#include "loottable.h"

#include <random>

#ifndef STEP_WIDTH
#define STEP_WIDTH 0.1f
#endif

#ifndef SIGMA
#define SIGMA 4
#endif

#ifndef SQRT2HALF
#define SQRT2HALF 0.7071067811865f
#endif

void LootTable::add_loot_table(int index, std::vector<int>& chances)
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

	int location = static_cast<int>(m_chances.size());
	m_indices.push_back({index, location});

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
	int max_indices_index				  = static_cast<int>(m_indices.size()) - 1;
	std::vector<LootTableIndices> indices = m_indices;

	if ( indices_count > m_indices.size() ) {
		count = static_cast<int>(m_indices.size());
	}

	while ( m_values.size() < count ) {
		m_values.push_back({-1, -1, -1});
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist_index(0, max_indices_index);
	std::uniform_int_distribution<> dist_value(0, 99);

	for ( int i = 0; i < count; i++ ) {
		int random_index = dist_index(gen);

		int current_index		= 0;
		int current_valid_index = 0;
		bool active				= true;

		while ( active ) {
			int current_index_in_range = current_index % max_indices_index;

			if ( indices.at(current_index_in_range).index != -1 ) {
				if ( current_valid_index == random_index ) {
					m_values.at(i).index					 = m_indices.at(current_index_in_range).index;
					m_values.at(i).value					 = dist_value(gen);
					indices.at(current_index_in_range).index = -1;
					active									 = false;
				}
				current_valid_index++;
			}
			current_index++;
		}
	}
}

void LootTable::calculate_curve_boundary()
{
	for ( auto& chance: m_chances ) {
		if ( chance.curve_boundary == 0 ) {
			for ( float i = -SIGMA; i <= SIGMA; i += STEP_WIDTH ) {
				float curve_value = static_cast<float>(0.5 * std::erff(SQRT2HALF * i) + 0.5) * 100;

				if ( curve_value <= static_cast<float>(chance.chance) ) {
					chance.curve_boundary = i;
				}
			}
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
					length = static_cast<int>(m_indices.size()) - m_indices.at(i).location;
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
