#include "mem.h"
#include "CPU.h"
#include "parser.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>

parser::parser ( mem& ram ) : memory ( ram )
{

}

bool parser::assemble ( const std::string& path )
{
	bool rc = false;
	std::string line;
	std::ifstream file ( path );

	if ( !file.is_open() ) {
		std::cout << "Cannot open file: " << path << '\n';
	} else {
		
		while ( std::getline ( file, line ) ) {

        		if ( line == ".data" && parseData ( file ) ) {
				
				rc = parseText ( file );
				break;
        		}
		}

		file.close();
	}

	return rc;
}


bool parser::parseData ( std::ifstream& file )
{
	bool rc = false;
	size_t pos;
	std::string line;
	labels.clear();
	data_address = 2048;

	while ( std::getline ( file, line ) ) {
		if ( line.empty() ) continue;
		char c = line [0];

        	if ( line == ".text" ) {
			rc = true;
			break;
		} else if ( !( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) ) ) continue;
		
		pos = line.find ( ';' );

		if ( pos != std::string::npos ) line.erase ( pos );

		pos = line.find ( ':' );
		std::string name = line.substr ( 0, pos );

		labels [ name ] = data_address;

		std::stringstream values ( line.substr ( pos + 1 ) );

		std::string token;
		
		while ( std::getline ( values, token, ',' ) ) {

			uint32_t number = static_cast <uint32_t> ( std::stoi ( token ) );

			uint8_t bytes [ 4 ] = {
				static_cast <uint8_t> ( number ),
				static_cast <uint8_t> ( number >> 8  ),
				static_cast <uint8_t> ( number >> 16 ),
				static_cast <uint8_t> ( number >> 24 )
			};

			memory.writeBytes ( data_address, bytes, 4 );

			data_address += 4;
        	}
	}

	return rc;
}


bool parser::parseText ( std::ifstream& file )
{
	bool rc = true;
	char c;
	size_t pos;
	std::string line;
	std::string command;
	text_address = 0;

	while ( std::getline ( file, line ) ) {
		if ( line.empty() ) continue;
		c = line [ 0 ];

		if ( !( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) ) ) continue;

		pos = line.find ( ';' );
		if ( pos != std::string::npos ) line.erase ( pos );

		pos = line.find ( ':' );
		
		if ( pos != std::string::npos ) {
			std::string name = line.substr ( 0, pos );
			labels [ name ] = text_address;
			continue;
		}

		std::stringstream str ( line );
		str >> command;

		auto it = cmd.find ( command );
		
		if ( it == cmd.end() ) {
			std::cout << "Unknown command: " << command << '\n';
			rc = false;
			break;
		}

		parseCommand ( it->second, str );

	}

	return rc;
}


bool parser::parseCommand ( uint8_t opcode, std::stringstream& args )
{
	uint8_t type = 0;
	uint8_t op1 = 0;
	uint32_t op2 = 0;

	char r, comma;
	int reg;

	std::string token;

	switch ( opcode ) {

		case 0: // LOAD
		{
			args >> r >> reg;
			op1 = static_cast < uint8_t > ( reg - 1 );

			args >> comma >> token;
			

			if ( token [ 1 ] == 'R' ) {
				op2 = static_cast < uint32_t > ( std::stoi ( token.substr ( 2, token.size() - 3 ) ) - 1 );
				type = static_cast < uint8_t > ( TYPE::INDIRECT );
			} else {
				token = token.substr ( 1, token.size() - 2 );
				auto it = labels.find ( token );
				op2 = it->second;
				type = static_cast < uint8_t > ( TYPE::DIRECT );
			}

			break;
		}

		case 1: // MOV 
		{
			args >> r >> reg;
			op1 = static_cast < uint8_t > ( reg - 1 );

			args >> comma >> token;

			auto it = labels.find ( token );

			if ( it != labels.end() ) {
				op2 = it->second;
			} else {
				op2 = static_cast < uint32_t > ( std::stoi ( token ) );
			}

			type = static_cast < uint8_t > ( TYPE::IMMEDIATE );
			break;
		}

		case 2: // ADD
		case 3: // SUB
			args >> r >> reg;
			op1 = static_cast < uint8_t > ( reg - 1 );

			args >> comma >> token;

			if ( token [ 0 ] == 'R' ) {
				op2 = static_cast < uint32_t > ( std::stoi ( token.substr ( 1 ) ) - 1 );
				type = static_cast < uint8_t > ( TYPE::REG );
			} else {
				op2 = static_cast < uint32_t > ( std::stoi ( token ) );
				type = static_cast < uint8_t > ( TYPE::IMMEDIATE );
			}

			break;

		case 4: // ADC
		case 5: // MUL
			args >> r >> reg;
			op1 = static_cast < uint8_t > ( reg - 1 );

			args >> comma >> r >> reg;
			op2 = static_cast < uint32_t > ( reg - 1 );

			type = static_cast < uint8_t > ( TYPE::REG );
			break;

		case 6: // JNZ
		{
			args >> token;
			auto it = labels.find ( token );
			op2 = it->second;
			type = static_cast < uint8_t > ( TYPE::IMMEDIATE );
			break;
		}
	}

	// Формируем 48-битную инструкцию
	uint64_t instruction =
        	( static_cast < uint64_t > ( opcode ) << 44 ) |
        	( static_cast < uint64_t > ( type ) << 42 ) |
        	( static_cast < uint64_t > ( op1 ) << 38 ) |
        	( static_cast < uint64_t > ( op2 ) << 6	);

	// Разбиваем инструкцию на 6 байт
	uint8_t bytes [ 6 ];

	for ( int i = 0; i < 6; i++ ) {
		bytes [ i ] = static_cast < uint8_t > ( instruction >> ( i * 8 ) );
	}

	memory.writeBytes ( text_address, bytes, 6 );

	text_address += 6;

	return true;
}
