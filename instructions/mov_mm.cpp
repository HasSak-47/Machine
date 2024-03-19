#define __INSTRUCTIONS_LOADER_HPP__
#include <Computer.hpp>

#define NAME MOV_MM

/**
 * moves the value from a memory address to another
 ** MOV (dest) (src)
 */
class NAME: public Instruction{
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		u16 src = mem.read_byte(cpu.ptr + 1) | (mem.read_byte(cpu.ptr + 2) << 8);
		u16 dst = mem.read_byte(cpu.ptr + 3) | (mem.read_byte(cpu.ptr + 4) << 8);
		mem.write_byte(dst, mem.read_byte(src));
	}

	const u8 get_size() override{ return 4; }
	const u8 get_code() override{ return 0x03; }
	const char* get_signature() override{ return "MOV %d %d"; }
};

PUSH_INSTRUCTION_MAKER(NAME)
