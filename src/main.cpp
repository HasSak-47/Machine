#include <filesystem>
#include <generic.hpp>
#include <iostream>
#include <vector>
#include <Computer.hpp>
#include <dlfcn.h>

const u8 raw_instructions[] = {
	0x01, 0x00, 0x40, 0x00, // mov r0 from 0x40
	0x01, 0x01, 0x41, 0x00, // mov r1 from 0x41

	0x10, // add r0 to r1 and store in r2
	0x02, 0x42, 0x00, 0x02, // mov r2 to 0x42

	0x11, // sub r0 to r1 and store in r2
	0x02, 0x43, 0x00, 0x02, // mov r2 to 0x43

	0x12, // mul r0 to r1 and store in r2
	0x02, 0x44, 0x00, 0x02, // mov r2 to 0x44

	0x13, // div r0 to r1 and store in r2
	0x02, 0x45, 0x00, 0x02, // mov r2 to 0x45

	0xff, // end
};

int main(){
	Instructions instructions;
	std::vector<void*> handles;

	// iterates all the ./inst/*.so files
	for (const auto& entry : std::filesystem::directory_iterator("./inst")){
		auto path = entry.path();
		auto handle = dlopen(path.c_str(), RTLD_LAZY);
		if (!handle){
			std::cerr << "dlopen failed: " << dlerror() << std::endl;
			continue;
		}
		std::cout << "loading: " << path << std::endl;

		auto add_instruction = (PushInstructionFunc)dlsym(handle, "push_instruction");
		if (!add_instruction){
			std::cerr << "dlsym failed: " << dlerror() << std::endl;
			dlclose(handle);
			continue;
		}

		std::cout << "adding instruction from handle: " << handle << std::endl;
		add_instruction(&instructions);
		std::cout << "instruction added!" << std::endl;
		handles.push_back(handle);
	}

	std::cout << "instructions loaded: " << instructions.size() << std::endl;
	for (auto& instruction : instructions){
		std::cout << "instruction: " << instruction->get_signature() ;
		std::cout << " code: " << std::hex << (int)instruction->get_code() << std::endl;
		std::cout << std::dec;
	}


	auto ram = RAM(256);
	std::cout << "writing data to virtual memory" << std::endl;
	ram.write_byte(0x40, 0x10);
	ram.write_byte(0x41, 0x05);
	std::cout << "writing instructions to virtual memory" << std::endl;
	ram.write(0, (u8*)raw_instructions, sizeof(raw_instructions));
	Computer computer{ram, {}, std::move(instructions)};

	std::cout << "running computer" << std::endl;
	std::cout << computer << std::endl;

	computer.run();

	std::cout << computer << std::endl;
	
	std::cout << "clearing instructions" << std::endl;
	computer.instructions.clear();
	std::cout << "closing handles" << std::endl;
	for (auto handle : handles){
		std::cout << "closing handle: " << handle << std::endl;
		dlclose(handle);
	}

	return 0;
}
