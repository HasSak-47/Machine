#include <logger.hpp>
#include <iostream>

using namespace wlog;

class Logger : public Writter{
public:
	void log(const std::string message) override{
		std::cout << message << std::endl;
	}
};
