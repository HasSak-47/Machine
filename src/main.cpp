#include <filesystem>
#include <generic.hpp>
#include <iostream>
#include <vector>
#include <Computer.hpp>
#include <dlfcn.h>

#include <logger.hpp>

#define LOAD_INSTRUCTIONS


int main(){
	wlog::Write write;
	write.add("stdout", std::make_unique<wlog::Logger>());
	write.add("stderr", std::make_unique<wlog::Logger>());
	Instructions instructions;
	std::vector<void*> handles;

	// iterates all the ./inst/*.so files
#ifdef LOAD_INSTRUCTIONS
	for (const auto& entry : std::filesystem::directory_iterator("./inst")){
		auto path = entry.path();
		auto handle = dlopen(path.c_str(), RTLD_LAZY);
		if (!handle){
			write["stderr"].write("dlopen failed: {}\n", dlerror());
			continue;
		}
		// std::cout << "loading: " << path << std::endl;

		auto add_instruction = (PushInstructionFunc)dlsym(handle, "push_instruction");
		if (!add_instruction){
			std::cerr << "dlsym failed: " << dlerror() << std::endl;
			dlclose(handle);
			continue;
		}

		// std::cout << "adding instruction from handle: " << handle << std::endl;
		add_instruction(&instructions);
		// std::cout << "instruction added!" << std::endl;
		handles.push_back(handle);
	}
#endif

	auto ram = RAM(256);
	// std::cout << "writing instructions to virtual memory" << std::endl;
	// ram.write(0, (u8*)raw_instructions, sizeof(raw_instructions));
	Computer computer{ram, {}, std::move(instructions)};
	std::cout << "reading asm" << std::endl;
	auto code = assemble("code.asm", computer.instructions);

	// std::cout << "writing instructions to virtual memory" << std::endl;
	ram.write(0, code.data(), code.size());
	std::cout << ram << std::endl;
	// std::cout << "running computer" << std::endl;
	computer.run();
	// std::cout << computer << std::endl;
	
	// std::cout << "clearing instructions" << std::endl;
	computer.instructions.clear();
	// std::cout << "closing handles" << std::endl;
	for (auto handle : handles){
		// std::cout << "closing handle: " << handle << std::endl;
		dlclose(handle);
	}

	return 0;
}
