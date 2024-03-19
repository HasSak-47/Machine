#include <iostream>
#define __INSTRUCTIONS_LOADER_HPP__
#include <Computer.hpp>

#define NAME MOV_RM

/**
 * moves the value from a memory address to a register
 ** MOV (dest) (src)
 */
class NAME: public Instruction{
	u8 dst = 0;
	u16 src = 0;
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		u8 dst = mem.read_byte(cpu.ptr + 1);
		u16 src = mem.read_byte(cpu.ptr + 2) | (mem.read_byte(cpu.ptr + 3) << 8);
		std::cout << "MOV_RM: " << (int)dst << " " << (int)src << std::endl;
		cpu.registers[dst] = mem.read_byte(src);
	}

	const u8 get_size() override{ return 3; }
	const u8 get_code() override{ return 0x01; }
	const char* get_signature() override{ return "MOV %dr %d"; }
};

PUSH_INSTRUCTION_MAKER(NAME)
