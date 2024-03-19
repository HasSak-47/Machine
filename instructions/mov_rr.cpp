#include <Computer.hpp>
#include <iostream>

class MOV_RR : public Instruction{
	u8 dst = 0;
	u8 src = 0;
	u8 count = 0;
	void act_on(CPU& cpu, MemoryDevice& mem) override{
		cpu.registers[dst] = cpu.registers[src];
	}
	void pass_byte(u8 byte) override{
		if (count == 0){
			dst = byte;
		}
		else if (count == 1){
			src = byte;
		}
		count++;
	}

	const u8 get_size() override{ return 2; }
	const u8 get_code() override{ return 0x00; }
	const char* get_signature() override{ return "MOV %dr %dr"; }
};

extern "C" {
	void add_instruction(Instructions* instructions){
		auto mov_rr = std::make_unique<MOV_RR>();
		instructions->push_back(std::move(mov_rr));
		std::cout << "added mov rr" << std::endl;
	}
}
