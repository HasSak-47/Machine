#define __INSTRUCTIONS_LOADER_HPP__
#include <Computer.hpp>

#define NAME CMP

/**
 * Compares reg0 and reg1
 */
class NAME: public Instruction{
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		if(cpu.registers[0] == cpu.registers[1]){
			cpu.cmp = CPU::CMP::EQUAL;
		}
		else
		if(cpu.registers[0] < cpu.registers[1]){
			cpu.cmp = CPU::CMP::LESS;
		}
		else {
			cpu.cmp = CPU::CMP::BIGGER;
		}
	}

	const u8 get_size() override{ return 0; }
	const u8 get_code() override{ return 0x20; }
	const char* get_signature() override{ return "CMP"; }
};

PUSH_INSTRUCTION_MAKER(NAME)
