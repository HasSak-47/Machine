#define INSTRUCTION_MAKER
#include <Computer.hpp>

#define NAME MOV_MR

static const InsS SIGNATURE = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Reg),
		(InsT)(InsT::Mem | InsT::Word),
	},
};

/**
 * moves the value from a register to a memory address
 ** MOV (dest) (src)
 */
INST_TEMPLATE(NAME, {
	u16 dst = mem.read_byte(cpu.ptr + 1) | (mem.read_byte(cpu.ptr + 2) << 8);
	u8 src = mem.read_byte(cpu.ptr + 3);
	mem.write_byte(dst, cpu.registers[src]);
}, 3, 0x02)

PUSH_INSTRUCTION_MAKER(NAME)
