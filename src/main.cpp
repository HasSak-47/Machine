#include <generic.hpp>
#include <memory.hpp>

#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <vector>

enum class InstructionCode : u8{
	// Move from A to B
	// MOV TYPES
	// 0x00 : Register to Register
	// 0x01 : Register to Memory
	// 0x02 : Memory to Register
	// 0x03 : Memory to Memory
	MOV_RR = 0x00, // DST SRC
	MOV_RM = 0x01, // RGS MEM
	MOV_MR = 0x02, // RGS MEM
	MOV_MM = 0x03, // DST SRC
	
	// OP reg0, reg1 -> reg2
    ADD, SUB,
    MUL, DIV,

	// Cmp reg0, reg1
    CMP,
	// JMP reg0 if Condition
	JNE, JE,
	JG, JGE,
	JL, JLE,

	// JMP reg0
	JMP,

	// HLT
    INT,
	DED,
};

class InstructionHandler{
public:
	InstructionCode code = InstructionCode::DED;
	virtual void write_to_memory(MemoryWriter& device) = 0;
	virtual ~InstructionHandler() = default;
};

class MOV_RR : public InstructionHandler{
private:
	u8 dst = 0;
	u8 src = 0;
public:
	MOV_RR(u8 dst, u8 src) : dst(dst), src(src) { code = InstructionCode::MOV_RR; }
	void write_to_memory(MemoryWriter& writer) override {
		writer.write((u8)code);
		writer.write(dst);
		writer.write(src);
	}
};

/** Moves from register to memory */
class MOV_RM : public InstructionHandler{
private:
	u8 reg = 0;
	u16 address = 0;
public:
	// moves from register to memory
	MOV_RM(u8 reg, u16 address) : reg(reg), address(address) { code = InstructionCode::MOV_RM; }
	void write_to_memory(MemoryWriter& writer) override {
		writer.write((u8)code);
		writer.write(reg);
		writer.write((u8)(address & 0xFF));
		writer.write((u8)(address >> 8));
	}
};

/** Moves from memory to register */
class MOV_MR : public InstructionHandler{
private:
	u8 reg = 0;
	u16 address = 0;
public:
	// moves to register from memory
	MOV_MR(u8 reg, u16 address) : reg(reg), address(address) { code = InstructionCode::MOV_MR; }
	void write_to_memory(MemoryWriter& writer) override {
		writer.write((u8)code);
		writer.write(reg);
		writer.write((u8)(address & 0xFF));
		writer.write((u8)(address >> 8));
	}
};

class MOV_MM : public InstructionHandler{
private:
	u16 address1 = 0;
	u16 address2 = 0;
public:
	MOV_MM(u16 address1, u16 address2) : address1(address1), address2(address2) { code = InstructionCode::MOV_MM; }
	void write_to_memory(MemoryWriter& writer) override {
		writer.write((u8)code);
		writer.write((u8)(address1 & 0xFF));
		writer.write((u8)(address1 >> 8));
		writer.write((u8)(address2 & 0xFF));
		writer.write((u8)(address2 >> 8));
	}
};


struct CPU {
	u8 registers[16] = {};
	u16 ptr = 0;
	bool end = false;

	void tick(MemoryDevice8& device);
	void mov_mr(MemoryDevice8& device){
		u8 reg = device.read(ptr++);
		u16 address = 0;
		for (u8 i = 0; i < 2; i++) 
			address |= device.read(ptr++) << (i * 8);
		registers[reg] = device.read(address);
	}
};

void CPU::tick(MemoryDevice8& device){
	InstructionCode instruction = (InstructionCode)device.read(ptr++);
	switch ( instruction ) {
		case InstructionCode::MOV_RR: {
			u8 reg1 = device.read(ptr++);
			u8 reg2 = device.read(ptr++);
			registers[reg1] = registers[reg2];
		} break;
		case InstructionCode::MOV_RM: {
			u8 reg = device.read(ptr++);
			u16 address = 0;
			for (u8 i = 0; i < 2; i++) 
				address |= device.read(ptr++) << (i * 8);
			device.write(address, registers[reg]);
		} break;
		case InstructionCode::MOV_MR: 
			this->mov_mr(device);
		break;
		case InstructionCode::MOV_MM: {
			u16 address1 = 0;
			for (u8 i = 0; i < 2; i++) 
				address1 |= device.read(ptr++) << (i * 8);
			u16 address2 = 0;
			for (u8 i = 0; i < 2; i++) 
				address2 |= device.read(ptr++) << (i * 8);
			device.write(address2, device.read(address1));
		}
		break;
		case InstructionCode::DED:
			end = true;
		default:
			break;
	}
}

std::ostream &operator<<(std::ostream &os, const CPU &cpu){
	os << "Registers: ";
	for (u8 i = 0; i < 16; i++) 
		os << std::hex << std::setw(2) << std::setfill('0') << (int)cpu.registers[i] << " ";
	os << "PTR: " << std::dec << (int)cpu.ptr;
	os << std::dec;
	return os;
}

i32 main(){
	RAM ram(256);
	MemoryWriter writer(ram);
	std::vector<InstructionHandler*> code;
	code.push_back(new MOV_MR(0, 128));
	code.push_back(new MOV_RR(1, 0));
	code.push_back(new MOV_RM(1, 129));
	code.push_back(new MOV_MM(128, 0));

	for (auto &instruction : code) {
		instruction->write_to_memory(writer);
		delete instruction;
	}

	writer.write((u8)InstructionCode::DED);
	ram.write(128, 0x40);
	CPU cpu;
	std::cout << ram << std::endl;
	while (!cpu.end) {
		std::cout << cpu << std::endl;
		cpu.tick(ram);
	}
	std::cout << ram << std::endl;

	return 0;
}
