#include <generic.hpp>
#include <iostream>
#include <proc_computer.hpp>

using namespace Proc;

class MOV_RR : public Instruction{
public:
	static const u64 __size = 2;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u8 reg1 = 0;
		u8 reg2 = 0;
		memory.read(cpu.ptr + 1, &reg1, 1);
		memory.read(cpu.ptr + 2, &reg2, 1);
		cpu.registers[reg1] = cpu.registers[reg2];
		cpu.ptr += __size;
	}

	u64 size() const override { return __size; }
};

class MOV_RM : public Instruction{
public:
	static const u64 __size = 3;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u8 reg = 0;
		u16 address = 0;
		memory.read(cpu.ptr + 1, &reg, 1);	
		memory.read(cpu.ptr + 2, (u8*)&address, 2);
		memory.write(address, (u8*)&cpu.registers[reg], 1);
		cpu.ptr += __size;
	}

	u64 size() const override { return __size; }
};

class MOV_MR : public Instruction{
public:
	static const u64 __size = 3;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u8 reg = 0;
		u16 address = 0;
		memory.read(cpu.ptr + 1, &reg, 1);	
		memory.read(cpu.ptr + 2, (u8*)&address, 2);
		u8 mem = 0;
		memory.read(address, &mem, 1);
		cpu.registers[reg] = mem;
		cpu.ptr += __size;
	}

	u64 size() const override { return __size; }
};

class MOV_MM : public Instruction{
public:
	static const u64 __size = 4;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u16 address1 = 0;
		u16 address2 = 0;
		memory.read(cpu.ptr + 1, (u8*)&address1, 2);
		memory.read(cpu.ptr + 3, (u8*)&address2, 2);
		u8 mem = 0;
		memory.read(address1, &mem, 1);
		memory.write(address2, &mem, 1);
		cpu.ptr += __size;
	}

	u64 size() const override { return __size; }
};

class DED : public Instruction {
public:
	static const u64 __size = 0;
	void execute(CPU& cpu, MemoryDevice& memory) override { cpu.end = true; }
	u64 size() const override { return __size; }
};

enum InstructionCode{
	MOV_RR_CODE = 0x00,
	MOV_RM_CODE = 0x01,
	MOV_MR_CODE = 0x02,
	MOV_MM_CODE = 0x03,

	DED_CODE = 0xff,
};

i32 main(){
	RAM ram(256);
	CPU cpu = {};
	Computer computer = { ram, cpu };
	cpu.add_instruction(MOV_RR_CODE, new MOV_RR());
	cpu.add_instruction(MOV_RM_CODE, new MOV_RM());
	cpu.add_instruction(MOV_MR_CODE, new MOV_MR());
	cpu.add_instruction(MOV_MM_CODE, new MOV_MM());
	cpu.add_end_instruction(DED_CODE, new DED());

	const u8 code[] = {
		MOV_MR_CODE, 0x00, 0x40, 0x00, // move to register 0 from memory at address 0x40
		MOV_RR_CODE, 0x01, 0x00, // move to register 1 from register 0
		MOV_RM_CODE, 0x01, 0x41, 0x01, // move to memory at address 0x40 from register 1
		MOV_MM_CODE, 0x00, 0x00, 0x40, 0x00, // move to memory at address 0x00 from memory at address 0x40
		DED_CODE,
	};

	ram.write(0, (u8*)code, sizeof(code));
	ram.write_byte(0x40, 0x10);
	std::cout << computer << '\n';
	while (!cpu.end){
		cpu.tick(ram);
	}


	return 0;
}
