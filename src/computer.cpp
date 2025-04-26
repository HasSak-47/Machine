#include <Computer.hpp>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>

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
	this->data = new u8[size] {255};
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
	instruction->act_on(cpu, memory);
	cpu.ptr += len;
}

void CPU::tick(MemoryDevice& memory, Instructions& instructions){
	u8 code = memory.read_byte(ptr);
	if(code == 255){
		end = true;
		return;
	}

	for(auto& instruction : instructions){
		u8 ins_code = instruction->get_code();
		if(ins_code == code){
			auto& sign = instruction->get_signature();
			std::string sig = sign.name;
			u64 t = sig.find(" ");
			if (t != std::string::npos)
				sig.resize(t);
			std::cout << std::hex << std::setw(2) << std::setfill(' ') ;
			std::cout << "running instruction: [" << std::setw(2) << std::setfill(' ') << (u32)ins_code << "]" << sig << " ";
			for(u32 i = 1; i <= instruction->get_size() - 1; ++i)
				std::cout << std::setw(2) << std::setfill(' ')<< (u64)memory.read_byte(ptr + i) << " ";
			std::cout<< std::dec << std::endl;

			run_instruction(instruction.get(), *this, memory);
			return;
		}
	}

	std::cout << "instruction not found: " << std::hex << (u64)code << std::endl;
	end = true;
}

void Computer::run(){
	while(!cpu.end){
		std::cout << *this << '\n';
		cpu.tick(memory, instructions);
		std::getchar();
	}
}

// ostream stuff
std::ostream& operator<<(std::ostream& os, const CPU& cpu){
	os << "CPU: " << std::endl;
	os << "count: " << cpu.count << std::endl;
	os << "end: " << cpu.end << std::endl;
	os << "ptr: " << std::hex << cpu.ptr << std::endl;
	auto c = (
		cpu.cmp == CPU::CMP::EQ? 
		   "EQ":
		cpu.cmp == CPU::CMP::LS? 
			"LS":
			"BG"
		);
	os << "cmp: " << c << std::endl;
	os << "registers: ";
	os << std::hex;
	for(auto reg : cpu.registers)
		os << (u64)reg << " ";
	os << std::dec << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, const MemoryDevice& mem){
	os << "RAM: " << std::endl;
	os << "size: " << mem.get_size() << std::endl;
	os << "x" << " | ";
	for (u64 i = 0; i < 16; ++i){
		os << std::hex << std::setw(2) << std::setfill(' ');
		os << i << " ";
	} os << std::endl;
	os << "--|";
	for (u64 i = 0; i < 16; ++i){
		os << "---";
	} os<<std::endl;
	for (u64 i = 0; i < mem.get_size(); ++i){
		if (i % 16 == 0){
			os << i / 16 << " | ";
		}
		os << std::hex << std::setw(2) << std::setfill(' ');
		os << (u64)mem.read_byte(i) << " ";

		if (i % 16 == 15)
			os << std::endl;
	}
	os << std::dec;
	return os;
}

std::ostream& operator<<(std::ostream& os, const Computer& computer){
	os << "Computer: " << std::endl;
	os << computer.memory << std::endl;
	os << computer.cpu << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, Instruction& instruction){
	const auto& sig = instruction.get_signature();
	u64 count = sig.params;
	os << sig.name << " ";
	for(u64 i = 0; i < count; ++i){
		auto arg = sig.args[i];
		os << arg << " ";
	}

	return os;
}

std::ostream &operator<<(std::ostream &os, const InstructionParamType& sig){
	using IPT = InstructionParamType;

	switch(sig & 0x0f){
		case IPT::Mem:
			os << "Mem";
			break;

		case IPT::Reg:
			os << "Reg";
			break;

		case IPT::Val:
			os << "Val";
			break;

		case IPT::Adr:
			os << "Adr";
			break;

		default:
			os << std::hex << "TE(" << (u64)(sig & 0x0f)<< ")" << std::dec;
	}
	switch(sig & 0xf0){
		case IPT::Byte:
			os << " Byte";
			break;

		case IPT::Word:
			os << " Word";
			break;
		default:
			os << std::hex << "LE(" << (u64)(sig & 0xf0)<< ")" << std::dec;
	}

	return os;
}
