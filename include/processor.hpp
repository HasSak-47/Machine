#ifndef __PROCESSOR_HPP__
#define __PROCESSOR_HPP__

#include "generic.hpp"

enum Instructions: u8{
	Add = 0x00,
	Sub = 0x01,
	Mul = 0x02,
	Div = 0x03,

	And = 0x04,
	Or  = 0x05,
	Xor = 0x06,
	Not = 0x07,
};

#endif // !__PROCESSOR_HPP__
