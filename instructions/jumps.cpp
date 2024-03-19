#define __INSTRUCTIONS_LOADER_HPP__
#include <Computer.hpp>

class JMP: public Instruction{
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		cpu.ptr = mem.read_byte(cpu.ptr + 1);
	}

	const u8 get_size() override{ return 2; }
	const u8 get_code() override{ return 0x30; }
	const char* get_signature() override{ return "JMP %d"; }
};

#define MAKE_JUMP_COND(NAME, COND, CODE, SIG) \
class NAME : public Instruction{ \
	void act_on(CPU& cpu, MemoryDevice& mem) override{ \
		if(COND)\
		cpu.ptr = mem.read_byte(cpu.ptr + 1);\
	} \
 \
	const u8 get_size() override{ return 2; } \
	const u8 get_code() override{ return CODE; } \
	const char* get_signature() override{ return SIG; } \
}; \

MAKE_JUMP_COND(JE , cpu.cmp == CPU::CMP::EQUAL , 0x31, "JE %d")
MAKE_JUMP_COND(JL , cpu.cmp == CPU::CMP::LESS  , 0x32, "JL %d")
MAKE_JUMP_COND(JB , cpu.cmp == CPU::CMP::BIGGER, 0x33, "JB %d")
MAKE_JUMP_COND(JLE, cpu.cmp == CPU::CMP::EQUAL || cpu.cmp == CPU::CMP::LESS, 0x34, "JLE %d")
MAKE_JUMP_COND(JGE, cpu.cmp == CPU::CMP::EQUAL || cpu.cmp == CPU::CMP::BIGGER, 0x35, "JGE %d")

extern "C"{
	void push_instruction(Instructions* i){
		i->push_back(std::make_unique<JMP>());
		i->push_back(std::make_unique<JE >());
		i->push_back(std::make_unique<JL >());
		i->push_back(std::make_unique<JB >());
		i->push_back(std::make_unique<JLE>());
		i->push_back(std::make_unique<JGE>());
	}
}
