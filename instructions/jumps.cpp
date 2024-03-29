#define INSTRUCTION_MAKER
#include <Computer.hpp>

static u16 new_address(CPU& cpu, MemoryDevice& mem){
	return mem.read_byte(cpu.ptr + 1) | (mem.read_byte(cpu.ptr + 2) << 8);
}

INST_TEMPLATE_FULL(
	JMP,
	{ cpu.ptr = new_address(cpu, mem);},
	0x30,
	"JMP", 1,
    {(InsT)(InsT::Word | InsT::Mem)}
)

INST_TEMPLATE_FULL(
	JMP_EQ,
	{ if(cpu.cmp == CPU::CMP::EQUAL) cpu.ptr = new_address(cpu, mem);},
	0x31,
	"JEQ", 1,
    {(InsT)(InsT::Word | InsT::Mem)}
)

INST_TEMPLATE_FULL(
	JMP_LS,
	{ if(cpu.cmp == CPU::CMP::LESS) cpu.ptr = new_address(cpu, mem);},
	0x32,
	"JLS", 1,
    {(InsT)(InsT::Word | InsT::Mem)}
)

INST_TEMPLATE_FULL(
	JMP_BG,
	{ if(cpu.cmp == CPU::CMP::BIGGER) cpu.ptr = new_address(cpu, mem);},
	0x33,
	"JBG", 1,
    {(InsT)(InsT::Word | InsT::Mem)}
)

INST_TEMPLATE_FULL(
	JMP_NE,
	{ if(cpu.cmp != CPU::CMP::EQUAL) cpu.ptr = new_address(cpu, mem);},
	0x34,
	"JNE", 1,
    {(InsT)(InsT::Word | InsT::Mem)}
)

INST_TEMPLATE_FULL(
	JMP_GE,
	{ if(cpu.cmp != CPU::CMP::LESS) cpu.ptr = new_address(cpu, mem);},
	0x35,
	"JGE", 1,
    {(InsT)(InsT::Word | InsT::Mem)}
)

INST_TEMPLATE_FULL(
	JMP_LE,
	{ if(cpu.cmp != CPU::CMP::BIGGER) cpu.ptr = new_address(cpu, mem);},
	0x36,
	"JLE", 1,
    {(InsT)(InsT::Word | InsT::Mem)}
)

extern "C" void push_instruction(Instructions& instructions){
	instructions.push_back(std::make_unique<JMP>());
	instructions.push_back(std::make_unique<JMP_EQ>());
	instructions.push_back(std::make_unique<JMP_LS>());
	instructions.push_back(std::make_unique<JMP_BG>());
	instructions.push_back(std::make_unique<JMP_NE>());
	instructions.push_back(std::make_unique<JMP_GE>());
	instructions.push_back(std::make_unique<JMP_LE>());
}

