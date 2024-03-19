#define __INSTRUCTIONS_LOADER_HPP__
#include <Computer.hpp>

#define NAME MOV_RR

/**
 * moves the value from one register to another
 ** MOV (dest)r (src)r
 */
class NAME : public Instruction{
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		u8 dst = mem.read_byte(cpu.ptr + 1);
		u8 src = mem.read_byte(cpu.ptr + 2);
		cpu.registers[dst] = cpu.registers[src];
	}

	const u8 get_size() override{ return 2; }
	const u8 get_code() override{ return 0x00; }
	const char* get_signature() override{ return "MOV %dr %dr"; }
};

PUSH_INSTRUCTION_MAKER(NAME)
