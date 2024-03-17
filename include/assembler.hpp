#ifndef __ASSEMBLER_HPP__
#define __ASSEMBLER_HPP__

#include "./generic.hpp"
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
		std::vector<u64> arg_ends = {};
		while(true){
			mut found = line.find(' ');
			if (found != std::string::npos) break;
			arg_ends.push_back(line.size() - 1);
		}
		std::vector<std::string> args = {};
		for (u64 i = 0; i < arg_ends.size(); i++){
			args.push_back(line.substr(0, arg_ends[i]));
			line = line.substr(arg_ends[i]);
		}
		for(auto arg : args){
			std::cout << "(" << arg << ") ";
		}
		std::cout << '\n';
	}
}

}

#endif
