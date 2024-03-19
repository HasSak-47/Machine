#define __INSTRUCTIONS_LOADER_HPP__
#include <Computer.hpp>

#define NAME MOV_MR

/**
 * moves the value from a register to a memory address
 ** MOV (dest) (src)
 */
class NAME: public Instruction{
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		u16 dst = mem.read_byte(cpu.ptr + 1) | (mem.read_byte(cpu.ptr + 2) << 8);
		u8 src = mem.read_byte(cpu.ptr + 3);
		mem.write_byte(dst, cpu.registers[src]);
	}

	const u8 get_size() override{ return 3; }
	const u8 get_code() override{ return 0x02; }
	const char* get_signature() override{ return "MOV %d %dr"; }
};

PUSH_INSTRUCTION_MAKER(NAME)
