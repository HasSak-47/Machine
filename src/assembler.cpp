#include "generic.hpp"
#include <Computer.hpp>
#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_map>

static std::vector<u8> get_args(const std::string& line){
	std::vector<u8> args;
	auto re_sig = std::regex("(0x)?[0-9a-f]+[rv]?");
	auto words_begin = std::sregex_iterator(line.begin(), line.end(), re_sig);
	for (std::sregex_iterator i = words_begin; i != std::sregex_iterator(); ++i){
		std::smatch match = *i;
		std::string match_str = match.str();
		if(match_str.find('r') != std::string::npos || match_str.find('v') != std::string::npos){
			match_str.pop_back();
			u8 arg = std::stoi(match_str, 0, 16);
			args.push_back(arg);
		}
		else{
			u16 arg = std::stoi(match_str, 0, 16);
			args.push_back(arg & 0xff);
			args.push_back(arg >> 8);
		}
		// std::cout << (i32)arg << std::endl;
	}

	return args;
}

static void to_regex(std::string& line){
	size_t pos = 0;
	while((pos = line.find("%d")) != std::string::npos)
		line.replace(pos, 2, "(0x)?[0-9a-f]+");
}

static Instruction* match_instruction(std::string& line, Instructions& instructions){
	for (auto& instruction : instructions){
		std::string sig = instruction->get_signature();
		to_regex(sig);
		auto re_sig = std::regex(sig);
		// check if the line matches the signature
		if(std::regex_match(line, re_sig)) 
			return instruction.get();
	}

	return nullptr;
}

static std::vector<u8> build_instruction(Instruction* instruction, const std::string& line){
	std::vector<u8> result;
	std::vector<u8> args = get_args(line);
	result.push_back(instruction->get_code());
	for(auto arg : args)
		result.push_back(arg);

	return result;
}

std::vector<u8> assemble(Instructions& instructions, const char* filename){
	std::vector<u8> bytes;
	std::ifstream file(filename);
	std::vector<std::string> lines = {};
	std::string line;
	while (std::getline(file, line))
		lines.push_back(line);

	std::unordered_map<std::string, u16> labels = {};

	// preprocess
	// find all labels and get their pos
	// size_t len = 0;
	// for(auto& line : lines){
	// 	if(line.find(':') != std::string::npos){
	// 		std::string label = line.substr(0, line.find(':'));
	// 		labels[label] = len;
	// 		continue;
	// 	}
	// 	Instruction* instr = match_instruction(line, instructions);
	// 	if(!instr){
	// 		std::cerr << "instruction not found: " << line << std::endl;
	// 		continue;
	// 	}
	// 	len += instr->get_size() + 1;
	// }

	// assembly
	for(auto& line : lines){
		// skip all labels
		if(line.find(':') != std::string::npos)
			continue;
		// replace all labels with their pos
		for(auto& label: labels){
			size_t pos = 0;
			while((pos = line.find(label.first)) != std::string::npos)
				line.replace(pos, label.first.length(), std::to_string(label.second));
		}
		std::cout << "line: " << line << std::endl;
		Instruction* instr = match_instruction(line, instructions);
		if(!instr){
			std::cerr << "instruction not found: " << line << std::endl;
			continue;
		}
		std::vector<u8> instr_bytes = build_instruction(instr, line);
		bytes.insert(bytes.end(), instr_bytes.begin(), instr_bytes.end());
	}
	bytes.push_back(0xff);

	return bytes;
}

void Computer::read_asm(const char* path){
	auto r = assemble(this->instructions, path);
	this->memory.write(0, r.data(), r.size());
}
