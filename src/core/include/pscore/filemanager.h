//
// Created by mariu on 09.01.2026.
//

#pragma once
#include <string>

class Filemanager
{

public:
	void ensurefileexists(std::string filename);

	Filemanager(std::string filename);

	void write(const std::string& mes);
	std::string load();

private:
	std::string filename;
};
