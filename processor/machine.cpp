#include <cstring>
#include <cmath>
#include <ctime>
#include <cerrno>
#include <cfenv>
#include "machine.h"

enum err_code
{
	ERR_NO,
	ERR_INVALID,
	ERR_DIV_BY_ZERO,
	ERR_VALUE_OVERFLOW,
	ERR_VALUE_UNDERFLOW
};

const char *error_strings[] =
{
	"No errors",
	"Invalid mathematical operation",
	"Division by zero",
	"Value is too big",
	"Value is too small"
};


err_code runtime_error ()
{
	if (std::fetestexcept (FE_INVALID))
		return ERR_INVALID;
	if (std::fetestexcept (FE_DIVBYZERO))
		return ERR_DIV_BY_ZERO;
	if (std::fetestexcept (FE_OVERFLOW))
		return ERR_VALUE_OVERFLOW;
	if (std::fetestexcept (FE_UNDERFLOW))
		return ERR_VALUE_UNDERFLOW;

	return ERR_NO;
}

VirtualMachine::VirtualMachine (std::istream &input, std::ostream &output)
	:cmd_counter  (0)
	,program_size (0)
	,program      (NULL)
	,in (input)
	,out (output)
{
	memset ((void*)registers, 0, sizeof (registers));
	srand (time (NULL));
}

VirtualMachine::VirtualMachine (const VirtualMachine &that)
	:calculations (that.calculations)
	,func_calls   (that.func_calls)
	,cmd_counter  (that.cmd_counter)
	,program_size (that.program_size)
	,in           (that.in)
	,out          (that.out)
{
	memcpy ((void*)registers, (void*)that.registers, sizeof (registers));
	memcpy ((void*)program, (void*)that.program, program_size);
}

VirtualMachine::~VirtualMachine ()
{
	if (program)
		free (program);
}

bool VirtualMachine::load (unsigned char *data, size_t length)
{
	if (program)
		free (program);

	if (length < 4)
	{
		out << "Program has an incorrect format" << std::endl;
		return false;
	}

	unsigned int program_header = (*data << 24) + (*(data + 1) << 16) + (*(data + 2) << 8) + *(data + 3);

	if (program_header != header)
	{
		if ((program_header & get_signature) != signature)
			out << "Signature of file is not coincide" << std::endl;
		else
		{
			out << "Incorrect program version " << std:: endl;
			out << "Program version: " << ((program_header & get_major_version) >> 8) << "." << (program_header & get_minor_version) << std::endl;
			out << "Processor version: " << (int)major_version << "." << (int)minor_version << std::endl;
		}

		return false;
	}
	memset ((void*)registers, 0, sizeof (registers));
	cmd_counter = 0;
	program_size = length - sizeof (header);
	program = (unsigned char*) malloc (program_size);
	memcpy ((void*)program, (void*)(data + sizeof (header)), program_size);
	return true;
}

bool VirtualMachine::execute ()
{
	std::cout.precision (10);
	while (cmd_counter < program_size)
	{
		switch (program[cmd_counter])
		{
		#define DEF_CMD(opcode, number, width, params, code) case number: code;\
        if (runtime_error()) { out << "Error: " << error_strings[runtime_error()] << std::endl; print_dump(); return false; } break;
		#include "../CmdList.h"
		#undef DEF_CMD
		default:
			out << "Error: Unknown command" << std::endl;
			print_dump ();
			return false;
		}
	}
	out << "Error: Command counter cross the limits (incorrect jump or lack of HALT in end)" << std::endl;
	print_dump ();
	return false;
}

void VirtualMachine::print_dump ()
{
	out << "MACHINE DUMP STATE" << std::endl;
	out << "Program size: " << program_size << ", current command: " << cmd_counter << " (";

	if (cmd_counter < program_size)
		switch (program[cmd_counter])
		{
			#define DEF_CMD(opcode, number, width, params, code) case number: out << #opcode;  break;
			#include "../CmdList.h"
			#undef DEF_CMD
		default:
			out << "Unknown command";
		}
	else
		out << "Out of program range";
	out << ")" << std::endl;
	out << "CALCULATIONS" << std::endl;
	calculations.print_dump (out);
	out << "FUNCTIONS CALLS" << std::endl;
	func_calls.print_dump (out);
	out << "Registers state:" << std::endl;
	for (int i = 0; i < num_registers; i++)
		out << "R" << i << ": " << registers[i] << std::endl;
}
