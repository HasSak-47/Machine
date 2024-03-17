#include <generic.hpp>
#include <iostream>
#include <proc_computer.hpp>

#include <assembler.hpp>

using namespace Proc;

enum InstructionCode{
	MOV_RR_CODE = 0x00,
	MOV_RM_CODE = 0x01,
	MOV_MR_CODE = 0x02,
	MOV_MM_CODE = 0x03,

	ADD_CODE = 0x14,
	SUB_CODE = 0x15,
	MUL_CODE = 0x16,
	DIV_CODE = 0x17,

	DED_CODE = 0xff,
};

#define MAKE_INSTRUCTION(NAME, SIZE, SOURCENAME, CODENAME, FUNC_BODY)\
class NAME : public Instruction{ public: \
	static const InstructionCode CODE = CODENAME;\
	static const char* SOURCE_NAME = "SOURCENAME";\
	u64 size() const override { return SIZE; }\
	const char* name() const override { return SOURCE_NAME; }\
	void execute(CPU& cpu, MemoryDevice& memory) override FUNC_BODY \
};

MAKE_INSTRUCTION(MOV_RR, 2, "MOV", MOV_RR_CODE, {
	u8 reg1 = memory.read_byte(cpu.ptr + 1);
	u8 reg2 = memory.read_byte(cpu.ptr + 2);
	cpu.registers[reg1] = cpu.registers[reg2];
});

// class MOV_RR : public Instruction{
// public:
// 	static const u64 SIZE = 2;
// 	static const InstructionCode CODE = MOV_RR_CODE;
// 	void execute(CPU& cpu, MemoryDevice& memory) override{
// 		u8 reg1 = memory.read_byte(cpu.ptr + 1);
// 		u8 reg2 = memory.read_byte(cpu.ptr + 2);
// 		cpu.registers[reg1] = cpu.registers[reg2];
// 	}
// 
// 	u64 size() const override { return SIZE; }
// };

class MOV_RM : public Instruction{
public:
	static const u64 SIZE = 3;
	static const InstructionCode CODE = MOV_RM_CODE;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u8 reg = 0;
		u16 address = 0;
		memory.read(cpu.ptr + 1, &reg, 1);	
		memory.read(cpu.ptr + 2, (u8*)&address, 2);
		memory.write(address, (u8*)&cpu.registers[reg], 1);
		

	}

	u64 size() const override { return SIZE; }
};

class MOV_MR : public Instruction{
public:
	static const u64 SIZE = 3;
	static const InstructionCode CODE = MOV_MR_CODE;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u8 reg = 0;
		u16 address = 0;
		memory.read(cpu.ptr + 1, &reg, 1);	
		memory.read(cpu.ptr + 2, (u8*)&address, 2);
		u8 mem = 0;
		memory.read(address, &mem, 1);
		cpu.registers[reg] = mem;
	}

	u64 size() const override { return SIZE; }
};

class MOV_MM : public Instruction{
public:
	static const u64 SIZE = 4;
	static const InstructionCode CODE = MOV_MM_CODE;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u16 address1 = 0;
		u16 address2 = 0;
		memory.read(cpu.ptr + 1, (u8*)&address1, 2);
		memory.read(cpu.ptr + 3, (u8*)&address2, 2);
		u8 mem = 0;
		memory.read(address1, &mem, 1);
		memory.write(address2, &mem, 1);
	}

	u64 size() const override { return SIZE; }
};

// operate content from r0 and r1 and store in r2
class AritmeticOperation : public Instruction {
public:
	static const u64 SIZE = 0;
	void execute(CPU& cpu, MemoryDevice& memory) override { 
		cpu.registers[2] = this->operate(cpu.registers[0], cpu.registers[1]);
	}
	u64 size() const override final { return SIZE; }
	virtual u8 operate(u8 a, u8 b) = 0;
};

#define MAKE_OPERATION(NAME, CODENAME, OP)\
class NAME : public AritmeticOperation{ public: \
	static const InstructionCode CODE = CODENAME; \
	u8 operate (u8 a, u8 b) override { return a OP b; } \
};

MAKE_OPERATION(AddOperation, ADD_CODE, +);
MAKE_OPERATION(SubOperation, SUB_CODE, -);
MAKE_OPERATION(MulOperation, MUL_CODE, *);
MAKE_OPERATION(DivOperation, DIV_CODE, /);


class DED : public Instruction {
public:
	static const u64 SIZE = 0;
	void execute(CPU& cpu, MemoryDevice& memory) override { cpu.end = true; }
	u64 size() const override { return SIZE; }
};

i32 main(){
	RAM ram(256);
	CPU cpu = {};
	Computer computer = { ram, cpu };
	cpu.add_instruction(MOV_RR_CODE, new MOV_RR());
	cpu.add_instruction(MOV_RM_CODE, new MOV_RM());
	cpu.add_instruction(MOV_MR_CODE, new MOV_MR());
	cpu.add_instruction(MOV_MM_CODE, new MOV_MM());

	cpu.add_instruction(ADD_CODE, new AddOperation());
	cpu.add_instruction(SUB_CODE, new SubOperation());
	cpu.add_instruction(MUL_CODE, new MulOperation());
	cpu.add_instruction(DIV_CODE, new DivOperation());

	cpu.add_end_instruction(DED_CODE, new DED());

	const u8 code[] = {
		MOV_MR_CODE, 0x00, 0x40, 0x00, // move to register 0 from memory at address 0x40
		MOV_RR_CODE, 0x01, 0x00, // move to register 1 from register 0
		MOV_RM_CODE, 0x01, 0x41, 0x00, // move to memory at address 0x41 from register 1
		MOV_MM_CODE, 0x00, 0x00, 0x40, 0x00, // move to memory at address 0x00 from memory at address 0x40
		DED_CODE,
	};

	ram.write(0, (u8*)code, sizeof(code));
	ram.write_byte(0x40, 0x10);
	std::cout << computer << '\n';
	std::cout << computer.cpu << '\n';
	std::cout << computer.memory << '\n';
	while (!cpu.end){
		cpu.tick(ram);
	}
	std::cout << computer.cpu << '\n';
	std::cout << computer.memory << '\n';


	return 0;
}
