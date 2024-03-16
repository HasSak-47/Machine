#include <generic.hpp>
#include <proc_computer.hpp>

using namespace Proc;

class MOV_RR : public Instruction{
public:
	static const u64 size = 2;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u8 reg1 = 0;
		u8 reg2 = 0;
		memory.read(cpu.ptr + 1, &reg1, 1);
		memory.read(cpu.ptr + 2, &reg2, 1);
		cpu.registers[reg1] = cpu.registers[reg2];
		cpu.ptr += size;
	}
};

class MOV_RM : public Instruction{
public:
	static const u64 size = 3;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u8 reg = 0;
		u16 address = 0;
		memory.read(cpu.ptr + 1, &reg, 1);	
		memory.read(cpu.ptr + 2, (u8*)&address, 2);
		memory.write(address, (u8*)&cpu.registers[reg], 1);
		cpu.ptr += size;
	}
};

class MOV_MR : public Instruction{
public:
	static const u64 size = 3;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u8 reg = 0;
		u16 address = 0;
		memory.read(cpu.ptr + 1, &reg, 1);	
		memory.read(cpu.ptr + 2, (u8*)&address, 2);
		u8 mem = 0;
		memory.read(address, &mem, 1);
		cpu.registers[reg] = mem;
		cpu.ptr += size;
	}
};

class MOV_MM : public Instruction{
public:
	static const u64 size = 4;
	void execute(CPU& cpu, MemoryDevice& memory) override{
		u16 address1 = 0;
		u16 address2 = 0;
		memory.read(cpu.ptr + 1, (u8*)&address1, 2);
		memory.read(cpu.ptr + 3, (u8*)&address2, 2);
		u8 mem = 0;
		memory.read(address1, &mem, 1);
		memory.write(address2, &mem, 1);
		cpu.ptr += size;
	}
};

enum InstructionCode{
	MOV_RR_CODE = 0x00,
	MOV_RM_CODE = 0x01,
	MOV_MR_CODE = 0x02,
	MOV_MM_CODE = 0x03
};

i32 main(){
	RAM ram(1024);
	Computer computer = {ram, CPU()};
	computer.cpu.add_instruction(MOV_RR_CODE, new MOV_RR());
	computer.cpu.add_instruction(MOV_RM_CODE, new MOV_RM());
	computer.cpu.add_instruction(MOV_MR_CODE, new MOV_MR());
	computer.cpu.add_instruction(MOV_MM_CODE, new MOV_MM());

	const u8 code[] = {
		MOV_MR_CODE, 0x00, 0x40, 0x00, // move to register 0 from memory at address 0x40
		MOV_RR_CODE, 0x01, 0x00, // move to register 1 from register 0
		MOV_RM_CODE, 0x01, 0x41, 0x01, // move to memory at address 0x40 from register 1
		MOV_MM_CODE, 0x00, 0x00, 0x40, 0x00, // move to memory at address 0x00 from memory at address 0x40
	};

	while (!computer.cpu.end){
		MemoryWriter writer(computer.memory);
		writer.write((u8*)code, sizeof(code));
		computer.cpu.tick(computer.memory);
	}

	return 0;
}
