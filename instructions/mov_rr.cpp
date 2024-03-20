#define INSTRUCTION_MAKER
#include <Computer.hpp>

#define NAME MOV_RR

static const InsS SIGNATURE = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Reg),
		(InsT)(InsT::Reg),
	},
};

/**
 * moves the value from one register to another
 ** MOV (dest)r (src)r
 */
INST_TEMPLATE(NAME, {
	u8 dst = mem.read_byte(cpu.ptr + 1);
	u8 src = mem.read_byte(cpu.ptr + 2);
	cpu.registers[dst] = cpu.registers[src];
}, 3, 0x00)

PUSH_INSTRUCTION_MAKER(NAME)
