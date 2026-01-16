//
// Created by mariu on 09.01.2026.
//

#pragma once
#include <string>

 class filemanager
 {
 	private:
	 std::string filename;
 	void ensurefileexists(std::string filename);

 	public:
 	filemanager(std::string filename);

 	void write();
 	std::string load();
 };
