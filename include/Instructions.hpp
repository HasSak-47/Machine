#ifndef __INSTRUCTIONS_HPP__
#define __INSTRUCTIONS_HPP__

#include "./generic.hpp"
#include <memory>
#include <unordered_map>

namespace Proc{

class Instruction;
class InstructionSet { 
public: 
	using PrimitiveSet = std::unordered_map<u8, std::unique_ptr<Instruction>>;
private:
	PrimitiveSet  _instructions;
public:
	PrimitiveSet::iterator find(u8 code){ return _instructions.find(code); }
	PrimitiveSet::iterator end(){ return _instructions.end(); }
	std::unique_ptr<Instruction>& operator[](u8 code){ return _instructions[code]; }
};
}

#endif
