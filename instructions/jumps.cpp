#define INSTRUCTION_MAKER
#include <Computer.hpp>

const InsS JMP_SIG = {
	.name = "JMP",
	.params = 1,
	.args = {
		(InsT)(InsT::Word | InsT::Mem),
	},
};

const InsS JEQ_SIG = {
	.name = "JEQ",
	.params = 1,
	.args = {
		(InsT)(InsT::Word | InsT::Mem),
	},
};

const InsS JLS_SIG = {
	.name = "JLS",
	.params = 1,
	.args = {
		(InsT)(InsT::Word | InsT::Mem),
	},
};

const InsS JGR_SIG = {
	.name = "JBG",
	.params = 1,
	.args = {
		(InsT)(InsT::Word | InsT::Mem),
	},
};

const InsS JNE_SIG = {
	.name = "JNE",
	.params = 1,
	.args = {
		(InsT)(InsT::Word | InsT::Mem),
	},
};

const InsS JGE_SIG = {
	.name = "JGE",
	.params = 1,
	.args = {
		(InsT)(InsT::Word | InsT::Mem),
	},
};

const InsS JLE_SIG = {
	.name = "JLE",
	.params = 1,
	.args = {
		(InsT)(InsT::Word | InsT::Mem),
	},
};

class JUMP_BASE : public Instruction{
public:
	virtual bool evaluate(CPU& cpu, MemoryDevice& mem) = 0;

	void act_on(CPU& cpu, MemoryDevice& mem) override{
		if(evaluate(cpu, mem))
			cpu.ptr = new_address(cpu, mem);
	}

	u8 new_address(CPU& cpu, MemoryDevice& mem){
		u16 v = mem.read_byte(cpu.ptr + 1) | (mem.read_byte(cpu.ptr + 2) << 8);
		return v - this->get_size();
	}
};

#define IMPL_JUMP(NAME, CODE, EVALUATOR) \
class NAME : public JUMP_BASE{\
	bool evaluate(CPU& cpu, MemoryDevice& mem) override{\
		EVALUATOR\
	}\
\
	const InsS& get_signature() override{ return NAME##_SIG; }\
	const u8 get_code() override{ return CODE; }\
};

using CCMP = CPU::CMP;

static bool is_flag_set(CPU& cpu, CCMP flag){
	return (u8)cpu.cmp & (u8)flag;
}


IMPL_JUMP(JMP, 0x40, {return true;});

IMPL_JUMP(JEQ, 0x41, {return is_flag_set(cpu, CCMP::EQ);});
IMPL_JUMP(JLS, 0x42, {return is_flag_set(cpu, CCMP::LS);});
IMPL_JUMP(JGR, 0x43, {return is_flag_set(cpu, CCMP::GR);});

IMPL_JUMP(JNE, 0x44, {return !is_flag_set(cpu, CCMP::EQ);});
IMPL_JUMP(JLE, 0x45, {return !is_flag_set(cpu, CCMP::LS);});
IMPL_JUMP(JGE, 0x46, {return !is_flag_set(cpu, CCMP::GR);});


extern "C" void push_instruction(Instructions& instructions){
	instructions.push_back(std::make_unique<JMP>());
	instructions.push_back(std::make_unique<JEQ>());
	instructions.push_back(std::make_unique<JLS>());
	instructions.push_back(std::make_unique<JGR>());
	instructions.push_back(std::make_unique<JNE>());
	instructions.push_back(std::make_unique<JLE>());
	instructions.push_back(std::make_unique<JGE>());
}

