#define INSTRUCTION_MAKER
#include <Computer.hpp>
#include <iostream>

#define NAME IO 

static const InsS SIGNATURE = {
	.name = "IO",
	.params = 3,
	.args = {
		(InsT)(InsT::Mem | InsT::Word),
		(InsT)(InsT::Val | InsT::Byte),
		(InsT)(InsT::Val | InsT::Byte),
	},
};

/**
 * acts on the string at the address in the first argument to the console.
 * if the second argument is 0, the string is printed to the console.
 * if the second argument is 1, the string is read from the console.
 * the third argument is the maximum length of the string.
 */
INST_TEMPLATE(NAME, {
	u64 addr =
		mem.read_byte(cpu.ptr + 1) |	
		mem.read_byte(cpu.ptr + 2) << 8
	;

	u64 len = mem.read_byte(cpu.ptr + 4);

	if(mem.read_byte(cpu.ptr + 3) == 0){
		for(u64 i = 0; i < len; i++)
			std::cout << mem.read_byte(addr + i);

		return;
	}

	if(mem.read_byte(cpu.ptr + 3) == 1){
		std::string str;
		std::cin >> str;

		for(u64 i = 0; i < len; i++)
			mem.write_byte(addr + i, i < str.size() ? str[i] : 0);
	}

}, 0x80)

PUSH_INSTRUCTION_MAKER(NAME)
