#pragma once

#include <vector>
#include <optional>



struct LootTableIndices
{
	int index;
	int location;
};

struct LootTableChances
{
	int chance;
	float curve_boundary;
};

struct LootTableValue
{
	int index;
	int value;
	int rarity;
};

class LootTable
{
public:
	void add_loot_table(int index, std::vector<int>& chances);

	void set_expected_value(float expected_value);

	std::vector<LootTableValue> loot_table_values(int count);

private:
	void random_values_from_loot_table(int indices_count);

	void calculate_curve_boundary();

	void calculate_rarity();


	std::vector<LootTableIndices> m_indices;
	std::vector<LootTableChances> m_chances;
	std::vector<LootTableValue> m_values;

	float m_expected_value = 0;
};

