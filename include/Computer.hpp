#ifndef __INSTRUCTIONS_HPP__
#define __INSTRUCTIONS_HPP__

#include "./generic.hpp"
#include <generic.hpp>
#include <memory>
#include <vector>

class Instruction;
using Instructions = std::vector<std::unique_ptr<Instruction>>;

class MemoryDevice{
public:
	virtual void write(u64 location, u8* data, u64 len) = 0;
	virtual void read(u64 location, u8* data, u64 len) const = 0;
	virtual ~MemoryDevice() = default;
	virtual u64 get_size() const = 0;

	void write_byte(u64 location, u8 data);
	u8 read_byte(u64 location);
};

class RAM : public MemoryDevice{
private:
	u8* data;
	u64 size;
public:
	RAM(u64 size);
	~RAM();

	void write(u64 location, u8* data, u64 len) override;
	void read(u64 location, u8* data, u64 len) const override;

	u64 get_size() const override;
};

class CPU{
public:
	// counts the number of cycles
	u64 count = 0;
	bool end = false;
	u64 ptr = 0;
	u8 registers[16];

	void tick(MemoryDevice& memory, Instructions& instructions);
};

class Computer{
public:
	MemoryDevice& memory;
	CPU& cpu;
	Instructions instructions;

	void run();
};

/**
 * this class holds the Instruction signature in the form of virtual const functions
 * and also will execute the instruction using non virtual functions
 */
class Instruction{
public:
	virtual void act_on(CPU& cpu, MemoryDevice& mem) = 0;
	virtual void pass_byte(u8 byte) = 0;

	// give the instruction the bytes it needs to execute
	void execute(CPU& cpu, MemoryDevice& mem){
		u8 size = this->get_size();
		for(u8 i = 0; i < size; i++){
			this->pass_byte(mem.read_byte(cpu.ptr + i));
		}
		act_on(cpu, mem);
	}

	/**
	 * all instruction signatures are in the next format:
	 * <instruction_name> <arg1> <arg2> ... <argN>
	 * ^(\w+) (%d[rm]?)*$
	 * @return the signature of the instruction
	 */
	virtual const char* get_signature() = 0;
	virtual const u8 get_code() = 0;
	virtual const u8 get_size() = 0;

	virtual ~Instruction() = default;
};

extern "C" {
	typedef void (*AddInstructionFunc)(Instructions* instructions);
}

#endif
