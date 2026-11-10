#include <iostream>
#include <fstream>
#include <string>

#include "mem.h"
#include "CPU.h"
#include "parser.h"

int main()
{
	mem ram;
	parser asm_parser ( ram );
	cpu processor ( ram );

	std::string path;
	int log_number = 1;

	while ( true ) {
		std::cout << "\nEnter path to assembly code (or 'exit'): ";
		std::getline ( std::cin, path );

		if ( path == "exit" )
			break;

		if ( path.empty() )
			continue;

		std::string log_name = "log" + std::to_string ( log_number ) + ".txt";

		std::ofstream log ( log_name );

		if ( !log.is_open() ) {
			std::cout << "Cannot create log file!\n";
			continue;
		}

		processor.reset();
		ram.clear();

		std::cout << "Starting assembly...\n";

		if ( !asm_parser.assemble ( path ) ) {
			std::cout << "Assembly error!\n";
			continue;
		}

		std::cout << "Assembly completed!\n";
		
		std::streambuf* old_buffer = std::cout.rdbuf();

		std::cout.rdbuf ( log.rdbuf() );
		processor.run();

		std::cout.rdbuf ( old_buffer );
		log.close();

		std::cout << "Execution finished. Log saved to " << log_name << '\n';

		log_number++;
	}

	std::cout << "Emulator terminated.\n";

	return 0;
}