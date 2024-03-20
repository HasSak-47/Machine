#define INSTRUCTION_MAKER
#include <Computer.hpp>

#define NAME MOV_RM

static const InsS SIGNATURE = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Reg),
		(InsT)(InsT::Mem | InsT::Word),
	},
};

/**
 * moves the value from a memory address to a register
 ** MOV (dest) (src)
 */
INST_TEMPLATE(NAME, {
	u8 dst = mem.read_byte(cpu.ptr + 1);
	u16 src = mem.read_byte(cpu.ptr + 2) | (mem.read_byte(cpu.ptr + 3) << 8);
	cpu.registers[dst] = mem.read_byte(src);
}, 3, 0x01)

PUSH_INSTRUCTION_MAKER(NAME)
