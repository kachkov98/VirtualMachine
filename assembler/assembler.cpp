#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <locale>
#include <vector>
#include <map>

//Version: 1.0
const unsigned char major_version = 1;
const unsigned char minor_version = 0;
const unsigned int signature = 'V' + ('M' << 8);
const unsigned int header = signature + (major_version << 16) + (minor_version << 24);

enum cmd_parameters
{
	EMP, //no parameters
	REG, //parameter is register - unsigned char
	JMP, //parameter is command offset - size_t
	VAL  //parameter is double
};

struct command
{
	std::string opcode;
	unsigned char number;
	unsigned char command_width;
	cmd_parameters parameter;
};

command commands[] =
{
	#define DEF_CMD(opcode, number, width, params, code)  {#opcode, number, width, params},
	#include "../CmdList.h"
    {"ERROR", 256, 1, EMP}
};

int main (int argc, char *argv[])
{
	std::vector <std::string> tokens;
	std::map <std::string, size_t> labels;
	
	if (argc != 3)
	{
		std::cout << "Incorrect program call: " << argv[0] << " <program name> <binary name>" << std::endl;
		return 1;
	}

	//parsing file on tokens
	std::ifstream asm_file (argv[1]);
	if (!asm_file.is_open ())
	{
		std::cout << "Error: can not open file " << argv[1] << std::endl;
		return 1;
	}

	std::string cur_string;
	while (std::getline (asm_file, cur_string))
	{
		size_t cur_pos = 0;
		bool is_space = true; //state of parser: true - space; false - part of token
		while (cur_pos < cur_string.size () && cur_string[cur_pos] != ';')
		{
			if (is_space)
			{
				if (!isspace (cur_string[cur_pos]) && cur_string[cur_pos] != ':')
				{
					tokens.push_back ("");
					tokens.back ().push_back (cur_string[cur_pos]);
					is_space = false;
				}
			}
			else
			{
				if (isspace (cur_string[cur_pos]) || cur_string[cur_pos] == ':')
				{
					if (cur_string[cur_pos] == ':')
					{
						tokens.back ().push_back (':');
					}
					is_space = true;
				}
				else
				{
					tokens.back ().push_back (cur_string[cur_pos]);
				}
			}
			cur_pos++;
		}
	}

	//1st pass: labels
	size_t cur_token = 0, cur_offset = 0;
	while (cur_token < tokens.size ())
	{
		if (tokens[cur_token].back () == ':') //find label
		{
			std::string label = tokens[cur_token].substr (0, tokens[cur_token].size () - 1);
			std::pair<std::map<std::string, size_t>::iterator, bool> result;

			result = labels.insert (std::pair<std::string, size_t> (label, cur_offset));

			if (!result.second)
			{
				std::cout << "Error: multiple definition of labels " << label << std::endl;
				return 0;
			}

			cur_token++;
		}
		else
		{
			size_t i = 0;
			while (i < std::extent<decltype(commands)>::value - 1)
			{
				std::string token_upper_case = tokens[cur_token];
				for (auto &iterator : token_upper_case)
					iterator = std::toupper (iterator, std::locale ());
				if (token_upper_case == commands[i].opcode)
				{
					cur_offset += commands[i].command_width;
					if (commands[i].parameter == EMP)
						cur_token++;
					else
						cur_token += 2;
					break;
				}

				i++;
			}

			if (commands[i].opcode == "ERROR")
			{
				std::cout << "Error: unknown command " << tokens[cur_token]
					      << ". It can be misprint or invalid arguments number" << std::endl;
				return 1;
			}
		}
	}

	//2nd pass: binary
	unsigned char *memblock = (unsigned char*) calloc (cur_offset, sizeof (unsigned char));
	assert (memblock && "Allocation error on 2nd pass of assembler");
	cur_token = 0;
	cur_offset = 0;
	
	while (cur_token < tokens.size ())
	{
		if (tokens[cur_token].back () == ':') //label
		{
			cur_token++;
		}
		else //command
		{
			for (size_t i = 0; i < std::extent<decltype(commands)>::value - 1; i++)
			{
				std::string token_upper_case = tokens[cur_token];
				for (auto &iterator : token_upper_case)
					iterator = std::toupper (iterator, std::locale ());
				if (token_upper_case == commands[i].opcode)
				{
					memblock[cur_offset] = commands[i].number;

					if (commands[i].parameter != EMP && cur_token == tokens.size () - 1)
					{
						std::cout << "Error: argument expected, but end of file is reached" << std::endl;
						free (memblock);
						return 1;
					}

					switch (commands[i].parameter)
					{
					case EMP:
						cur_token++;
						cur_offset += commands[i].command_width;
						break;
					case REG:
						if (tokens[cur_token + 1][0] != 'R')
						{
							std::cout << "Error: incorrect register name " << tokens[cur_token + 1] << std::endl;
							free (memblock);
							return 1;
						}
						int register_num;

						try
						{
							register_num = std::stoi (tokens[cur_token + 1].substr (1, tokens[cur_token + 1].size () - 1));
						}
						catch (const std::invalid_argument& a)
						{
							std::cout << "Error: invalid argument " << a.what ()  << " token: " << tokens[cur_token] << std::endl;
							free (memblock);
							return 1;
						}
						catch (const std::out_of_range& b)
						{
							std::cout << "Error: out of range " << b.what () << " token: " << tokens[cur_token] << std::endl;
							free (memblock);
							return 1;
						}
						
						if (register_num < 0 || register_num > 255)
						{
							std::cout << "Error: incorrect register number " << tokens[cur_token + 1] << std::endl;
							free (memblock);
							return 1;
						}
						memblock[cur_offset + 1] = (unsigned char)register_num;
						cur_token += 2;
						cur_offset += commands[i].command_width;
						break;
					case JMP:
						size_t jump;
						try
						{
							jump = labels.at (tokens[cur_token + 1]);
						}
						catch (std::out_of_range)
						{
							std::cout << "Error: label " << tokens[cur_token + 1] << " not found. Command: " << tokens[cur_token] << std::endl;
							free (memblock);
							return 1;
						}
						memcpy ((void *)(memblock + cur_offset + 1), (void *)&jump, sizeof (jump));
						cur_token += 2;
						cur_offset += commands[i].command_width;
						break;
					case VAL:
						double value;
						try
						{
							value = std::stod (tokens[cur_token + 1]);
						}
						catch (const std::invalid_argument& a)
						{
							std::cout << "Error: invalid argument " << a.what () << " token: " << tokens[cur_token] << std::endl;
							free (memblock);
							return 0;
						}
						catch (const std::out_of_range& b)
						{
							std::cout << "Error: out of range " << b.what () << " token: " << tokens[cur_token] << std::endl;
							free (memblock);
							return 1;
						}
						memcpy ((void*)(memblock + cur_offset + 1), (void*)&value, sizeof (value));
						cur_token += 2;
						cur_offset += commands[i].command_width;
						break;
					}
					break;
				}
			}
		}
	}

	std::ofstream binary (argv[2], std::ios::out | std::ios::binary);
	if (!binary.is_open ())
	{
		std::cout << "Error: can not open " << argv[2] << std::endl;
		free (memblock);
		return 1;
	}
	//writing header
	binary.write ((const char*)&header, sizeof (header));
	//writing program
	binary.write ((const char*)memblock, cur_offset);
	binary.close ();
	free (memblock);
	return 0;
}
