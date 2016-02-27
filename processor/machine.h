#pragma once

#include "stack.h"

//Version: 1.0
const unsigned char major_version = 1;
const unsigned char minor_version = 0;
const unsigned int signature = ('V'<<24) + ('M' << 16);
const unsigned int header = signature + (major_version << 8) + minor_version;
const unsigned int get_signature     = (255 << 24) + (255 << 16);
const unsigned int get_major_version = (255 << 8);
const unsigned int get_minor_version = 255;

const int num_registers = 256;

class VirtualMachine
{
private:
	Stack<double> calculations;
	Stack<size_t> func_calls;
	double registers[num_registers];
	size_t cmd_counter;
	size_t program_size;
	unsigned char *program;
	std::istream &in;
	std::ostream &out;
public:
	VirtualMachine (std::istream &input = std::cin,
					std::ostream &output = std::cout);
	VirtualMachine (const VirtualMachine &that);
	~VirtualMachine ();
	bool load (unsigned char *data, size_t length);
	bool execute ();
	void print_dump ();
};