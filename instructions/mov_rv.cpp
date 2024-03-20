#define INSTRUCTION_MAKER
#include <Computer.hpp>

#define NAME MOV_RV

static const InsS SIGNATURE = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Reg),
		(InsT)(InsT::Val),
	},
};

/**
 * moves the value from next memory address to a register
 ** MOV (dest)r (src)v
 */
INST_TEMPLATE(NAME, {
	u8 dst = mem.read_byte(cpu.ptr + 1);
	u8 src = mem.read_byte(cpu.ptr + 2);
	cpu.registers[dst] = src;
}, 2, 0x04)

PUSH_INSTRUCTION_MAKER(NAME)
