#include <Computer.hpp>
#include <iostream>

void MemoryDevice::write_byte(u64 location, u8 data){ this->write(location, &data, 1); }
u8 MemoryDevice::read_byte(u64 location) const {
	u8 byte = 0;
	this->read(location, &byte, 1);
	return byte;
}

void RAM::write(u64 location, u8* data, u64 len){
	for (u64 i = 0; i < len; i++)
		this->data[location + i] = data[i];
}

RAM::RAM(u64 size){
	this->size = size;
	this->data = new u8[size];
}

RAM::~RAM(){
	delete[] data;
}

void RAM::read(u64 location, u8* data, u64 len) const{
	for (u64 i = 0; i < len; i++)
		data[i] = this->data[location + i];
}

u64 RAM::get_size() const{
	return size;
}

static void run_instruction(Instruction* instruction, CPU& cpu, MemoryDevice& memory){
	u64 len = instruction->get_size();
	for (u64 i = 1; i <= len; i++){
		u8 byte = memory.read_byte(cpu.ptr + i);
		instruction->pass_byte(byte);
	}
	instruction->act_on(cpu, memory);
}

void CPU::tick(MemoryDevice& memory, Instructions& instructions){
	std::cout << "in cycle: " << this->count++ << std::endl;
	u8 code = memory.read_byte(ptr);
	if(code == 255){
		end = true;
		return;
	}

	for(auto& instruction : instructions){
		u8 ins_code = instruction->get_code();
		if(ins_code == code){
			ptr += instruction->get_size() + 1;
			return;
		}
	}

	std::cout << "instruction not found: " << (u64)code << std::endl;
	end = true;
}

void Computer::run(){
	while(!cpu.end){
		cpu.tick(memory, instructions);
	}
}

// ostream stuff
std::ostream& operator<<(std::ostream& os, const CPU& cpu){
	os << "CPU: " << std::endl;
	os << "count: " << cpu.count << std::endl;
	os << "end: " << cpu.end << std::endl;
	os << "ptr: " << cpu.ptr << std::endl;
	os << "registers: ";
	for(auto reg : cpu.registers)
		os << (u64)reg << " ";
	os << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, const MemoryDevice& mem){
	os << "RAM: " << std::endl;
	os << "size: " << mem.get_size() << std::endl;
	// for (u64 i = 0; i < mem.get_size(); ++i){
	// 	// writes it in hex
	// 	os << std::hex << (u64)mem.read_byte(i) << " ";
	// 	if (i % 16 == 15)
	// 		os << std::endl;
	// }
	return os;
}

std::ostream& operator<<(std::ostream& os, const Computer& computer){
	os << "Computer: " << std::endl;
	os << computer.memory << std::endl;
	os << computer.cpu << std::endl;
	return os;
}
