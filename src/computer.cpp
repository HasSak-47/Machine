#include <Computer.hpp>
#include <iostream>

void MemoryDevice::write_byte(u64 location, u8 data){ this->write(location, &data, 1); }
u8 MemoryDevice::read_byte(u64 location){
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
			instruction->execute(*this, memory);
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
