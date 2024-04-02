#include <logger.hpp>
#include <iostream>

using namespace wlog;

void Stdout::log(const std::string message) {
	std::cout << message << std::endl;
}

void Debug::log(const std::string message) {
	if (debug)
		std::cout << message << std::endl;
}
