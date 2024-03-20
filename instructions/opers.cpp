#define INSTRUCTION_MAKER
#include <Computer.hpp>

#define MAKE_OPER(NAME, CODE, OPER) \
static const InsS NAME##SIG= { \
	.name = #NAME, \
	.params = 0, \
	.args = {}, \
}; \
class NAME : public Instruction{ \
	void act_on(CPU& cpu, MemoryDevice& mem) override{ \
		cpu.registers[2] = cpu.registers[0] OPER cpu.registers[1]; \
	} \
\
	const u8 get_size() override{ return 0; } \
	const u8 get_code() override{ return CODE; } \
	const InsS get_signature() override{ return NAME##SIG; } \
}; \
\

/**
 * adds the contents of reg 0 to reg 1 and stores the result in reg 2
 */

MAKE_OPER(ADD, 0x10, +)
MAKE_OPER(SUB, 0x11, -)
MAKE_OPER(MUL, 0x12, *)
MAKE_OPER(DIV, 0x13, /)

extern "C"{
	void push_instruction(Instructions* i){
		i->push_back(std::make_unique<ADD>());
		i->push_back(std::make_unique<SUB>());
		i->push_back(std::make_unique<MUL>());
		i->push_back(std::make_unique<DIV>());
	}
}
