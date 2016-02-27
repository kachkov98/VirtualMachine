#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "machine.h"

int main (int argc, char *argv[])
{
	
	if (argc < 2)
	{
		std::cout << "Incorrect program call: " << argv[0] << " <program names>" << std::endl;
		return 1;
	}

	VirtualMachine processor;

	for (int i = 1; i < argc; i++)
	{
		std::ifstream file;
		file.open (argv[i], std::ios::in|std::ios::binary|std::ios::ate);
		if (!file.is_open ())
		{
			std::cout << "Error: can't open " << argv[i] << std::endl;
			return 1;
		}
		std::streampos size = file.tellg ();
		unsigned char *data = (unsigned char*)malloc ((size_t)size);
		assert (data && "Allocation error in Main function");
		file.seekg (0, std::ios::beg);
		file.read ((char *)data, size);
		file.close ();

		if (!processor.load (data, (size_t)size))
		{
			std::cout << "Error: can not get data from " << argv[i] << std::endl;
			return 0;
		}

		clock_t t1 = clock ();
		if (!processor.execute ())
		{
			std::cout << "Program " << argv[i] << " finished unsuccessfully" << std::endl;
			processor.print_dump ();
			return 0;
		}
		std::cout << "Execution time: " << (clock () - t1) * 1000 / CLOCKS_PER_SEC << " ms" << std::endl;
		free (data);
		file.close ();
	}
	return 0;
}
