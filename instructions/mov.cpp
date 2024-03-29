#include <iostream>
#define INSTRUCTION_MAKER
#include <Computer.hpp>

const InsS MOV_MM_SIG = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Mem | InsT::Word),
		(InsT)(InsT::Mem | InsT::Word),
	},
};

const InsS MOV_MR_SIG = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Reg | InsT::Byte),
		(InsT)(InsT::Mem | InsT::Word),
	},
};

const InsS MOV_RM_SIG = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Reg | InsT::Byte),
		(InsT)(InsT::Mem | InsT::Word),
	},
};

const InsS MOV_RR_SIG = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Reg | InsT::Byte),
		(InsT)(InsT::Reg | InsT::Byte),
	},
};

const InsS MOV_RV_SIG = {
	.name = "MOV",
	.params = 2,
	.args = {
		(InsT)(InsT::Reg | InsT::Byte),
		(InsT)(InsT::Val | InsT::Byte),
	},
};

#define IMPL_MOV(NAME, ACTOR, SIZE, CODE, SIGNATURE) \
class NAME : public Instruction{\
	void act_on(CPU& cpu, MemoryDevice& mem) override{ ACTOR }\
	const u8 get_code() override{ return CODE; }\
	const InsS& get_signature() override{ return SIGNATURE; }\
};

// move to dst from src
IMPL_MOV(MOV_MM, {
	u16 src = mem.read_word(cpu.ptr + 1);
	u16 dst = mem.read_word(cpu.ptr + 3);
	mem.write_byte(dst, mem.read_byte(src));
}, 4, 0x03, MOV_MM_SIG)

// move from a register to a memory address
IMPL_MOV(MOV_MR, {
	u16 dst = mem.read_word(cpu.ptr + 1);
	u8 src = mem.read_byte(cpu.ptr + 3);
	mem.write_byte(dst, cpu.registers[src]);
}, 3, 0x02, MOV_MR_SIG)

// move from a memory address to a register
IMPL_MOV(MOV_RM, {
	u8 dst = mem.read_byte(cpu.ptr + 1);
	u16 src = mem.read_word(cpu.ptr + 2);
	cpu.registers[dst] = mem.read_byte(src);
}, 3, 0x01, MOV_RM_SIG)

// move from one register to another
IMPL_MOV(MOV_RR, {
	u16 dst = mem.read_byte(cpu.ptr + 1);
	u16 src = mem.read_byte(cpu.ptr + 2);
	cpu.registers[dst] = cpu.registers[src];
}, 3, 0x00, MOV_RR_SIG)

// move from a value to a register
IMPL_MOV(MOV_RV, {
	u16 dst = mem.read_byte(cpu.ptr + 1);
	u16 val = mem.read_byte(cpu.ptr + 2);
	cpu.registers[dst] = val;
}, 2, 0x04, MOV_RV_SIG)

extern "C"{
	void push_instruction(Instructions* i){
		i->push_back(std::make_unique<MOV_MM>());
		i->push_back(std::make_unique<MOV_MR>());
		i->push_back(std::make_unique<MOV_RM>());
		i->push_back(std::make_unique<MOV_RR>());
		i->push_back(std::make_unique<MOV_RV>());
	}
}
