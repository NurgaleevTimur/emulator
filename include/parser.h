#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <unordered_map>


static const std::unordered_map < std::string, uint8_t > cmd = {
	{ "LOAD", 0 },
	{ "MOV", 1 },
	{ "ADD", 2 },
	{ "SUB", 3 },
	{ "ADC", 4 },
	{ "MUL", 5 },
	{ "JNZ", 6 },
	{ "HALT", 7 }
};

class mem;

class parser
{
	private:
		bool parseData ( std::ifstream& file );
		bool parseText ( std::ifstream& file );
		bool parseCommand ( uint8_t opcode, std::stringstream& args );

		std::unordered_map < std::string, uint16_t > labels;
		uint16_t data_address = 2048;
		uint16_t text_address = 0;

		mem& memory;

	public:
		parser( mem& ram );

		bool assemble ( const std::string& path );
};

#endif // PARSER_H