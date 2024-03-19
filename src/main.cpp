#include <filesystem>
#include <generic.hpp>
#include <iostream>
#include <vector>
#include <Computer.hpp>
#include <dlfcn.h>

const u8 instructions[] = {
	0x00, 0x01, 0x00,
	0xff, // end
};

int main(){
	RAM ram(1024);
	CPU cpu;
	cpu.registers[0] = 0x17;
	ram.write(0, (u8*)instructions, sizeof(instructions));

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


	Computer computer{ram, cpu, std::move(instructions)};
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
