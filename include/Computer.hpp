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
	u8 read_byte(u64 location) const;
};

class RAM : public MemoryDevice{
private:
	u8* data = nullptr;
	u64 size = 0;
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
	enum class CMP: u8{
		BIGGER,
		EQUAL,
		LESS,
	} cmp;
	u8 registers[16] = {};
	u64 ptr = 0;

	void tick(MemoryDevice& memory, Instructions& instructions);
};

class Computer{
public:
	MemoryDevice& memory;
	CPU cpu;
	Instructions instructions;

	void run();
	void read_asm(const char* path);
};

/**
 * this class holds the Instruction signature in the form of virtual const functions
 * and also will execute the instruction using non virtual functions
 */
class Instruction{
public:
	virtual void act_on(CPU& cpu, MemoryDevice& mem) = 0;

	/**
	 * all instruction signatures are in the next format:
	 * <instruction_name> <arg1> <arg2> ... <argN>
	 ** regex: ^(\w+) (%d[rm]?)*$
	 **         ^name  ^arg format
	 * @return the signature of the instruction
	 */
	virtual const char* get_signature() = 0;
	virtual const u8 get_code() = 0;
	virtual const u8 get_size() = 0;

	virtual ~Instruction() = default;
};


// guard for adding ostream outs only to the main file
#ifndef __INSTRUCTION_LOADER_HPP__
#define __INSTRUCTION_LOADER_HPP__

std::ostream& operator<<(std::ostream& os, const CPU& cpu);
std::ostream& operator<<(std::ostream& os, const MemoryDevice& mem);
std::ostream& operator<<(std::ostream& os, const Computer& computer);

#endif


extern "C" {
	typedef void (*PushInstructionFunc)(Instructions* instructions);
}

#define PUSH_INSTRUCTION_MAKER(name) \
extern "C" { \
	void push_instruction(Instructions* instructions){ \
		auto _inst= std::make_unique<name>(); \
		instructions->push_back(std::move(_inst)); \
	} \
}

#endif
