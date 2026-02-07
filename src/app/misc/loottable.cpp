#include "loottable.h"

#include <random>

void LootTable::add_loot_table(int index, std::vector<int>& chances)
{
	for ( auto element : m_indices ) {
		if ( element.index == index ) {
			return;
		}
	}

	m_indices.push_back({index, static_cast<int>(m_chances.size())});

	//Glockenkurve einfügen

	for ( auto chance : chances ) {
		m_chances.push_back(chance);
	}
}

std::optional<std::vector<int>> LootTable::loot_table_at_index(int index)
{
	for (int i = 0; auto element : m_indices ) {
		if ( element.index == index ) {
			std::vector<int> loot_table;

			if ( m_indices.size() - 1 == i ) {
				for ( int j = 0; j < m_indices.size() - m_indices.at(i).location; j++ ) {
					loot_table.push_back(m_chances.at(j + m_indices.at(i).location));
				}
			}
			else {
				for ( int k = 0; k < m_indices.at(i + 1).location - m_indices.at(i).location; k++ ) {
					loot_table.push_back(m_chances.at(k + m_indices.at(i).location));
				}
			}
			return loot_table;
		}
		i++;
	}

	return std::nullopt;
}

void LootTable::random_indices_from_loot_table(int indices_count)
{
	int count = indices_count;

	if ( indices_count > m_indices.size() ) {
		count = static_cast<int>(m_indices.size());
	}

	while ( m_values.size() < count ) {
		m_values.push_back({-1,-1});
	}

	for ( auto& value : m_values ) {
		value = {-1,-1};
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0,static_cast<int>(m_indices.size()) - 1);

	for ( int i = 0; i < count; i++ ) {
		int random_index = dist(gen);

		int j = 0;
		int k = 0;
		bool search = true;
		while ( search ) {
			if ( m_values.at(j).index == -1 ) {
				if ( k == random_index ) {
					m_values.at(k).index = j;
					search = false;
				}
				k++;
			}
			j++;
		}
	}
}

void LootTable::random_values_from_loot_table()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0,99);

	for ( auto& value : m_values ) {
		for (int i = 0; auto element : m_indices ) {
			if ( value.index == element.index ) {
				int random_value = dist(gen);
				value.value = 0;

				if ( m_indices.size() - 1 == i ) {
					for ( int j = 0; j < m_indices.size() - m_indices.at(i).location; j++ ) {
						if ( m_chances.at(j + m_indices.at(i).location) < random_value ) {
							value.value = j;
						}
					}
				}
				else {
					for ( int k = 0; k < m_indices.at(i + 1).location - m_indices.at(i).location; k++ ) {
						if ( m_chances.at(k + m_indices.at(i).location) < random_value ) {
							value.value = k;
						}
					}
				}
			}
			i++;
		}
	}
}
