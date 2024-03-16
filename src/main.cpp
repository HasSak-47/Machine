#include <generic.hpp>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>

enum class Instruction : u8{
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
	

    ADD, SUB,
    MUL, DIV,

    CMP,
	JNE, JE,
	JG, JGE,
	JL, JLE,

	JMP,

    INT,
	DED,
};

class InstructionHandler{
public:
	Instruction code = Instruction::DED;
	u32 arg1_size = 0;
	u32 arg2_size = 0;
};


let mov_rr = InstructionHandler{Instruction::MOV_RR, 1, 1};
let mov_rm = InstructionHandler{Instruction::MOV_RM, 1, 2};
let mov_mr = InstructionHandler{Instruction::MOV_MR, 1, 2};
let mov_mm = InstructionHandler{Instruction::MOV_MM, 2, 2};


class MemoryDevice8{
public:
	virtual u8 read(u64 address) const = 0 ;
	virtual void write(u64 address, u8 data) = 0;
	virtual u64 get_size() const = 0;
};

std::ostream &operator<<(std::ostream &os, const MemoryDevice8 &device){
	u64 size = device.get_size();
	os << "MemoryDevice8 " << size << std::endl;
	// print hex dump of mem
	for (u64 i = 0; i < size; i++) {
		// set hex output with double 0
		os << std::hex << std::setfill('0') << std::setw(2);
		os << (int)device.read(i) << " ";
		if (i % 16 == 15) os << std::endl;
	}
	// reset output
	os << std::dec << std::endl;

	return os;
}

struct RAM : public MemoryDevice8{
	u8* data;
	u64 size;

	RAM(u64 size){
		this->size = size;
		this->data = new u8[size];
	}
	~RAM(){ delete[] data; }

	u8 read(u64 address) const override
		{ return data[address]; }
	void write(u64 address, u8 data) override
		{ this->data[address] = data; }
	u64 get_size() const override
		{ return size; }
};

struct MemoryWriter{
	u64 ptr = 0;
	MemoryDevice8& device;
	MemoryWriter(MemoryDevice8& device) : device(device) {}
	void write(u8 data){ device.write(ptr++, data); }
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
	Instruction instruction = (Instruction)device.read(ptr++);
	switch ( instruction ) {
		case Instruction::MOV_RR: {
			u8 reg1 = device.read(ptr++);
			u8 reg2 = device.read(ptr++);
			registers[reg1] = registers[reg2];
		} break;
		case Instruction::MOV_RM: {
			u8 reg = device.read(ptr++);
			u16 address = 0;
			for (u8 i = 0; i < 2; i++) 
				address |= device.read(ptr++) << (i * 8);
			registers[reg] = device.read(address);
		} break;
		case Instruction::MOV_MR: 
			this->mov_mr(device);
		break;
		case Instruction::MOV_MM: {
			u16 address1 = 0;
			for (u8 i = 0; i < 2; i++) 
				address1 |= device.read(ptr++) << (i * 8);
			u16 address2 = 0;
			for (u8 i = 0; i < 2; i++) 
				address2 |= device.read(ptr++) << (i * 8);
			device.write(address2, device.read(address1));
		}
		break;
		case Instruction::DED:
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
	MemoryWriter writer = MemoryWriter(ram);
	writer.write(MOV_MR);
	writer.write(0);   // move to register 0
	writer.write(128); // from memory address 128
	writer.write(0);
	writer.write(MOV_RR);
	writer.write(1); // move to register 1
	writer.write(0); // from register 0
	writer.write(MOV_MM);
	writer.write(128); // from address 128
	writer.write(0);
	writer.write(0); // move to address 0
	writer.write(0);
	
	
	writer.write(DED); // end

	ram.write(128, 0x10);


	CPU cpu;
	while (!cpu.end) {
		std::cout << cpu << std::endl;
		cpu.tick(ram);
	}
	std::cout << ram << std::endl;
	return 0;
}
