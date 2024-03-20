#define INSTRUCTION_MAKER
#include <Computer.hpp>

#define NAME MOV_MM

static const InsS SIGNATURE = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Mem | InsT::Word),
		(InsT)(InsT::Mem | InsT::Word),
	},
};

/**
 * moves the value from a memory address to another
 ** MOV (dest) (src)
 */
INST_TEMPLATE(NAME, {
	u16 src = mem.read_byte(cpu.ptr + 1) | (mem.read_byte(cpu.ptr + 2) << 8);
	u16 dst = mem.read_byte(cpu.ptr + 3) | (mem.read_byte(cpu.ptr + 4) << 8);
	mem.write_byte(dst, mem.read_byte(src));
}, 4, 0x03)

PUSH_INSTRUCTION_MAKER(NAME)
