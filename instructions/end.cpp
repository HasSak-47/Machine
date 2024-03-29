#define INSTRUCTION_MAKER
#include <Computer.hpp>

#define NAME END

static const InsS SIGNATURE = {
	.name = "END",
	.params = 0,
	.args = {},
};

INST_TEMPLATE(NAME, {
	cpu.end = true;
}, 0xff)

PUSH_INSTRUCTION_MAKER(NAME)
