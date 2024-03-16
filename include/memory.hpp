#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include "./generic.hpp"
#include <ostream>
#include <iomanip>

class MemoryDevice8{
public:
	virtual u8 read(u64 address) const = 0 ;
	virtual void write(u64 address, u8 data) = 0;
	virtual u64 get_size() const = 0;

	friend std::ostream &operator<<(std::ostream &os, const MemoryDevice8 &device){
		u64 size = device.get_size();
		os << "MemoryDevice8 " << size << std::endl;
		// print hex dump of mem
		for (u64 i = 0; i < size; i++) {
			// set hex output with double 0
			os << std::hex << std::setfill('0') << std::setw(2);
			os << (int)device.read(i) << " ";
			if (i % 16 == 15) os << std::endl;
		}
		// reset output
		os << std::dec << std::endl;
	
		return os;
	}
};


struct MemoryWriter{
	u64 ptr = 0;
	MemoryDevice8& device;
	MemoryWriter(MemoryDevice8& device) : device(device) {}
	void write(u8 data){ device.write(ptr++, data); }
};

struct RAM : public MemoryDevice8{
	u8* data;
	u64 size;

	RAM(u64 size){
		this->size = size;
		this->data = new u8[size];
	}
	~RAM(){ delete[] data; }

	u8 read(u64 address) const override
		{ return data[address]; }

	void write(u64 address, u8 data) override
		{ this->data[address] = data; }

	u64 get_size() const override
		{ return size; }

};
#endif // __MEMORY_HPP__
