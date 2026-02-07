#pragma once

#include <vector>
#include <optional>

struct LootTableIndices
{
	int index;
	int location;
};

struct LootTableValue
{
	int index;
	int value;
};

class LootTable
{
public:
	void add_loot_table(int index, std::vector<int>& chances);

	std::optional<std::vector<int>> loot_table_at_index(int index);

private:
	void random_indices_from_loot_table(int indices_count);

	void random_values_from_loot_table();

	std::vector<LootTableIndices> m_indices;
	std::vector<int> m_chances;
	std::vector<LootTableValue> m_values;
};

