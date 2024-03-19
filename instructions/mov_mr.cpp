#define __INSTRUCTIONS_LOADER_HPP__
#include <Computer.hpp>

#define NAME MOV_MR

/**
 * moves the value from a register to a memory address
 ** MOV (dest) (src)
 */
class NAME: public Instruction{
	u8 dst = 0;
	u16 src = 0;
	u8 count = 0;
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		cpu.registers[dst] = mem.read_byte(src);
	}
	void pass_byte(u8 byte) override{
		if (count == 0){
			src = byte;
		}
		else if (count == 1){
			src |= byte << 8;
		}
		else if (count == 2){
			dst = byte;
		}
		count++;
	}

	const u8 get_size() override{ return 3; }
	const u8 get_code() override{ return 0x01; }
	const char* get_signature() override{ return "MOV %d %dr"; }
};

PUSH_INSTRUCTION_MAKER(NAME)
