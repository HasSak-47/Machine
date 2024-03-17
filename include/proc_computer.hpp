#ifndef __PROC_COMPUTER_HPP__
#define __PROC_COMPUTER_HPP__

#include "./generic.hpp"
#include "./Instructions.hpp"
#include <iomanip>
#include <ostream>

namespace Proc{

class MemoryDevice;
class CPU;
class Instruction;

class MemoryDevice{
public:
	virtual void write(u64 location, u8* data, u64 len) = 0;
	virtual void read(u64 location, u8* data, u64 len) const = 0;
	virtual ~MemoryDevice() = default;
	virtual u64 get_size() const = 0;

	void write_byte(u64 location, u8 data){ this->write(location, &data, 1); }
	u8 read_byte(u64 location){
		u8 byte = 0;
		this->read(location, &byte, 1);
		return byte;
	}
};

inline std::ostream& operator<<(std::ostream& os, const MemoryDevice& device){
	u64 size = device.get_size();
	os << "MemoryDevice: " << std::endl;
	os << "Size: " << size << std::endl;
	for (u64 i = 0; i < size; i++){
		u8 data = 0;
		device.read(i, &data, 1);
		os << std::setw(2) << (u64)data << " ";
		if( i % 16 == 15)
			os << std::endl;
	}
	return os;
}

struct RAM : public MemoryDevice{
	u8* data;
	u64 size;

	RAM(u64 size){
		this->size = size;
		this->data = new u8[size];
	}
	~RAM(){ delete[] data; }

	void write(u64 location, u8* data, u64 len) override{
		for (u64 i = 0; i < len; i++){
			this->data[location + i] = data[i];
		}
	}

	void read(u64 location, u8* data, u64 len) const override{
		for (u64 i = 0; i < len; i++){
			data[i] = this->data[location + i];
		}
	}

	u64 get_size() const override{
		return size;
	}
};

struct MemoryWriter{
	u64 ptr = 0;
	MemoryDevice& device;
	MemoryWriter(MemoryDevice& device) : device(device) {}
	void write(u8* data, u64 len){ device.write(ptr++, data, len); }
	void write(u8 data){ device.write(ptr++, &data, 1); }
};

class CPU{
public:
	u64 ptr = 0;
	u8 registers[16] = {};
	bool end = false;
	InstructionSet instructions;
	u8 end_code = 0xff;
	
	void tick(MemoryDevice& device);
	void add_instruction(u8 code, Instruction* instruction);
	void add_end_instruction(u8 code, Instruction* instruction){
		instructions[code] = std::unique_ptr<Instruction>(instruction);
	}
};

inline std::ostream& operator<<(std::ostream& os, const CPU& cpu){
	os << "CPU: " << std::endl;
	os << "Registers: " << std::endl;
	for (u64 i = 0; i < 16; i++){
		os << "R" << i << ": " << (u64)cpu.registers[i] << " ";
	}
	os << "PTR: " << cpu.ptr << std::endl;
	return os;
}

struct Computer{
	MemoryDevice& memory;
	CPU& cpu;
};

class Instruction{
public:
	virtual void execute(CPU& cpu, MemoryDevice& device) = 0;
	virtual u64 size() const = 0;
	virtual char* name() const = 0;
	virtual ~Instruction() = default;
};

inline std::ostream& operator<<(std::ostream& os, const Computer& computer){
	os << "Computer: " << std::endl;
	os << "objdump: " << std::endl;
	u64 temporal_ptr = 0;
	while (temporal_ptr < computer.memory.get_size()){
		u8 data = 0;
		computer.memory.read(temporal_ptr, &data, 1);
		if(data == computer.cpu.end_code){
			os << "0x" << std::hex << (int)data << std::endl;
			break;
		}
		let instruction = computer.cpu.instructions.find(data);

		if (instruction != computer.cpu.instructions.end()){
			u64 size = instruction->second->size() + 1;
			u8 data = 0;
			for (u64 i = 0; i < size; ++i){
				computer.memory.read(temporal_ptr + i, &data, 1);
				os << "0x" << std::hex << (int)data << " ";
			}
			os << std::endl;
			temporal_ptr += size;
		}
		else{
			temporal_ptr++;
		}
	}
	return os;
}

}

#endif
