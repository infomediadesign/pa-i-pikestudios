//
// Created by mariu on 09.01.2026.
//

#include <filesystem>
#include <fstream>
#include <pscore/filemanager.h>

Filemanager::Filemanager(std::string filename)
{
	filename = filename + ".txt";
}

void Filemanager::write(const std::string& mes)
{
	// check whether the file exists and open the file
	ensurefileexists(filename);
	std::ofstream outfile(filename);
	if ( !outfile.is_open() )
		return;

	outfile << mes + "\n";

	outfile.close();
};

std::string Filemanager::load()
{
	// does the file exist? create it if not. / delete highscore / open txt file and check
	ensurefileexists(filename);
	std::ifstream infile(filename);
	if ( !infile.is_open() )
		return "";

	std::string file;

	std::string line;
	while ( infile >> line ) {
		file.append(line);
	}
	infile.close();
	return file;
}
void Filemanager::ensurefileexists(std::string filename)
{
	namespace fs = std::filesystem;
	std::error_code ec;
	fs::path p(filename.c_str());
	if ( p.has_parent_path() ) {
		fs::create_directory(p.parent_path(), ec);
	}

	if ( !fs::exists(p) ) {
		std::ofstream f(filename, std::ios::out);
	}
}
