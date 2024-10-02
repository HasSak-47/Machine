#define INSTRUCTION_MAKER
#include <Computer.hpp>

#define NAME CMP

static const InsS SIGNATURE = {
	.name = "CMP",
	.params = 0,
	.args = {},
};

/**
 * Compares reg0 and reg1
 */
INST_TEMPLATE(NAME, {
	if(cpu.registers[0] == cpu.registers[1]){
		cpu.cmp = CPU::CMP::EQ;
	}
	else
	if(cpu.registers[0] < cpu.registers[1]){
		cpu.cmp = CPU::CMP::LS;
	}
	else
	if(cpu.registers[0] < cpu.registers[1]) {
		cpu.cmp = CPU::CMP::GR;
	}
}, 0x20)

PUSH_INSTRUCTION_MAKER(NAME)
