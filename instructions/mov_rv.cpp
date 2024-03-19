#define __INSTRUCTIONS_LOADER_HPP__
#include <Computer.hpp>

#define NAME MOV_RV

/**
 * moves the value from next memory address to a register
 ** MOV (dest)r (src)v
 */
class NAME : public Instruction{
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		u8 dst = mem.read_byte(cpu.ptr + 1);
		u8 src = mem.read_byte(cpu.ptr + 2);
		cpu.registers[dst] = src;
	}

	const u8 get_size() override{ return 2; }
	const u8 get_code() override{ return 0x04; }
	const char* get_signature() override{ return "MOV %dr %dv"; }
};

PUSH_INSTRUCTION_MAKER(NAME)
