#ifndef __ASSEMBLER_HPP__
#define __ASSEMBLER_HPP__

#include "./generic.hpp"
#include "./Instructions.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace Asm{

inline void assemble(std::string path){
	std::ifstream source(path.c_str());
	if (!source.is_open()){
		std::cerr << "Error: Could not open file " << path << std::endl;
		return;
	}

	std::string line;
	std::cout << "Assembling " << path << "..." << std::endl;
	while (std::getline(source, line)){
		std::cout << line << std::endl;

	}
}

}

#endif
