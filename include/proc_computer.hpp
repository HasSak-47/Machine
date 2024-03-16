#ifndef __PROC_COMPUTER_HPP__
#define __PROC_COMPUTER_HPP__

#include "./generic.hpp"
#include <memory>
#include <unordered_map>

namespace Proc{

class MemoryDevice;
class CPU;
class Instruction;

class MemoryDevice{
public:
	virtual void write(u64 location, u8* data, u64 len) = 0;
	virtual void read(u64 location, u8* data, u64 len) const = 0;
};

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
};

struct MemoryWriter{
	u64 ptr = 0;
	MemoryDevice& device;
	MemoryWriter(MemoryDevice& device) : device(device) {}
	void write(u8* data, u64 len){ device.write(ptr++, data, len); }
	void write(u8 data){ device.write(ptr++, &data, 1); }
};

using InstructionSet = std::unordered_map<u8, std::unique_ptr<Instruction>>;

class CPU{
public:
	u64 ptr = 0;
	u8 registers[16] = {};
	bool end = false;
	InstructionSet instructions;
	
	void tick(MemoryDevice& device);
	void add_instruction(u8 code, Instruction* instruction);
};

struct Computer{
	MemoryDevice& memory;
	CPU cpu;
};

class Instruction{
public:
	static const u64 size;
	virtual void execute(CPU& cpu, MemoryDevice& device) = 0;
};

}

#endif
