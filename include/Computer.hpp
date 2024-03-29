#ifndef __INSTRUCTIONS_HPP__
#define __INSTRUCTIONS_HPP__

#include "./generic.hpp"
#include <generic.hpp>
#include <memory>
#include <vector>

class Instruction;
using Instructions = std::vector<std::shared_ptr<Instruction>>;

class MemoryDevice{
public:
	virtual void write(u64 location, u8* data, u64 len) = 0;
	virtual void read(u64 location, u8* data, u64 len) const = 0;
	virtual ~MemoryDevice() = default;
	virtual u64 get_size() const = 0;

	void write_byte(u64 location, u8 data);
	u8 read_byte(u64 location) const;
	u16 read_word(u64 location) const {
		u16 data = 0;
		read(location, (u8*)&data, 2);
		return data;
	}
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
};

/**
 * this class holds the Instruction signature in the form of virtual const functions
 * and also will execute the instruction using non virtual functions
 */
enum InstructionParamType : u8{
	// typeof 0xn0

	Mem = 0x01,
	Reg = 0x02,
	Val = 0x04,
	Adr = 0x08,

	// lens 0x0n

	Byte = 0x10,
	Word = 0x20,

	ERROR = 0xff,
};

struct InstructionSignature{
	const char* name;
	const size_t params;
	const InstructionParamType args[];

	InstructionSignature(const InstructionSignature& sig) = delete;
};

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
	virtual const InstructionSignature& get_signature() = 0;
	virtual const u8 get_code() = 0;

	/**
	 * @return the size of the instruction in bytes
	 */
	const u8 get_size() {
		let& signature = get_signature();
		u64 size = 1;
		for(u64 i = 0; i < signature.params; i++){
			size += (signature.args[i] & 0xf0) >> 4;
		}
		return size;
	}

	virtual ~Instruction() = default;
};

std::ostream& operator<<(std::ostream& os, Instruction& instruction);
std::ostream& operator<<(std::ostream& os, const InstructionParamType& sig);

std::vector<u8> assemble(const std::string& code, Instructions& instructions);


// for adding ostream outs only to the VM executable
#ifdef INSTRUCTION_MAKER 
using InsT = InstructionParamType;
using InsS = InstructionSignature;

#define INST_TEMPLATE_FULL(NAME, ACTOR, CODE, SIGN_NAME, SIGN_COUNT, SIGN_ARR) \
const InsS NAME##_SIGNATURE = { SIGN_NAME, SIGN_COUNT, SIGN_ARR };\
class NAME : public Instruction\
{\
	void act_on(CPU& cpu, MemoryDevice& mem) override{ ACTOR }\
	const u8 get_code() override{ return CODE; }\
	const InstructionSignature& get_signature() override{ return NAME##_SIGNATURE; }\
};

#define INST_TEMPLATE(NAME, ACTOR, CODE) \
class NAME : public Instruction\
{\
	void act_on(CPU& cpu, MemoryDevice& mem) override{ ACTOR }\
	const u8 get_code() override{ return CODE; }\
	const InstructionSignature& get_signature() override{ return SIGNATURE; }\
};

// for adding a basic instruction template only to the instruction shared objects
#else // INSTRUCTION_MAKER
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
