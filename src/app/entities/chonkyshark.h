#pragma once

#include <entities/shark.h>

class ChonkyShark : public Shark
{
public:
	ChonkyShark();
	~ChonkyShark();

	std::optional<std::vector<Vector2>> bounds() const override;

	void on_hit() override;
	
private:
	bool hurt = false;
};
