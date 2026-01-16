//
// Created by mariu on 09.01.2026.
//

#include <filesystem>
#include <fstream>
#include  <pscore/filemanager.h>

 filemanager::filemanager(std::string filename)
{
	filename = filename + ".txt";
}

void filemanager::write()
{

};
std::string filemanager::load()
{

}
void filemanager::ensurefileexists(std::string filename)
{
	namespace fs = std::filesystem;
	std::error_code ec;
	fs::path p(filename.c_str());
	if (p.has_parent_path()) {
		fs::create_directory(p.parent_path(), ec);
	}

	if (!fs::exists(p)) {
		std::ofstream f(filename,std::ios::out);
	}
}

