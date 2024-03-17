
#include <proc_computer.hpp>

using namespace Proc;

void CPU::tick(MemoryDevice& memory){
	u8 code = 0;
	memory.read(ptr, &code, 1);
	if (instructions.find(code) != instructions.end()){
		instructions[code]->execute(*this, memory);
	}
	else{
		end = true;
	}
}

void CPU::add_instruction(u8 code, Instruction* instruction){
	instructions[code] = std::unique_ptr<Instruction>(instruction);
}
