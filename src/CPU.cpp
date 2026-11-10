#include "CPU.h"
#include <cstring>
#include <iostream>


cpu::cpu ( mem& ram ) : memory ( ram )
{
	memset ( R, 0, sizeof ( R ) );
}


void cpu::reset()
{
	memset ( R, 0, sizeof ( R ) );
	RF = 0;
	PC = 0;
	IR = 0;

	halted = 0;
	flag = 0;
	
	in1 = 0;
	in2 = 0;
	low_res = 0;
	high_res = 0;
	result = 0;
}

void cpu::fetch()
{
	memory.readBytes ( PC, reinterpret_cast <uint8_t*> ( &IR ), 6 );
}

void cpu::decode()
{
	opcode = static_cast <OPCODE> ( ( IR >> 44 ) & 0xF );
	type = static_cast <TYPE> ( ( IR >> 42 ) & 0x3 );
	op1 = ( IR >> 38 ) & 0xF;
	op2 = ( IR >> 6 ) & 0xFFFFFFFF;

	if ( opcode != OPCODE::JNZ && opcode != OPCODE::HALT && opcode != OPCODE::LOAD && opcode != OPCODE::MOV ) {

		in1 = R [ op1 ];

		switch ( type )
		{
			case TYPE::REG:
				in2 = R [ op2 ];
				break;

			case TYPE::IMMEDIATE:
				in2 = op2;
				break;
		}
	}
}

void cpu::execute()
{

	switch ( opcode ) {
	
		case OPCODE::LOAD:
			switch ( type )
			{
				case TYPE::DIRECT:
					memory.readBytes ( op2, reinterpret_cast <uint8_t*> (&result), 4 );
					break;

				case TYPE::INDIRECT:
					memory.readBytes ( R [ op2 ], reinterpret_cast <uint8_t*> (&result), 4 );
					break;
			}
			flag = 0;
			break;
			
		case OPCODE::MOV:
			result = op2;
			flag = 0;
			break;

		case OPCODE::ADD:
			result = in1 + in2;

			flag = 0;

			if ( result < in1 )
				flag |= 0x1; // CF

			if ( ( ~( in1 ^ in2 ) & ( in1 ^ result ) ) & 0x80000000 )
				flag |= ( 1 << 1 ); // OF
	
			if ( result == 0 ) 
				flag |= ( 1 << 2 ); // ZF
			
			break;
		
		case OPCODE::SUB:
			result = in1 - in2;

			flag = 0;

			if ( in1 < in2 )
				flag |= 0x1; // CF

			if ( ( ( in1 ^ in2 ) & ( in1 ^ result ) ) & 0x80000000 )
				flag |= ( 1 << 1 ); // OF

			if ( result == 0 )
				flag |= ( 1 << 2 ); // ZF
			break;
	
		case OPCODE::ADC:
			result = in1 + in2;

			flag = 0;

			if ( result < in1 )
				flag |= 0x1; // CF

			result += static_cast <uint32_t> ( RF & 0x1 );
			
			if ( ( ~( in1 ^ in2 ) & ( in1 ^ result ) ) & 0x80000000 )
				flag |= ( 1 << 1 ); // OF

			if ( result == 0 )
				flag |= ( 1 << 2 ); // ZF
			break;

		case OPCODE::MUL:
		{
			uint32_t a_low = in1;
			uint32_t a_high = 0;
			uint32_t b = in2;
			uint32_t t;
			low_res = 0;
			high_res = 0;
			flag = 0;

			if ( in1 == 0 || in2 == 0 ) {
				flag |= ( 1 << 2 ); // ZF
				break;
			}
			
			bool sign = ( ( in1 >> 31 ) ^ ( in2 >> 31 ) ) != 0;

			if ( in1 & 0x80000000 )
				a_low = ~a_low + 1;

    			if ( in2 & 0x80000000 )
				b = ~b + 1;

			if ( a_low < b ) {
				t = a_low;
				a_low = b;
				b = t;
			}

			if ( a_low <= 0xFFFF && b <= 0xFFFF ) {
				low_res = a_low * b;
				high_res = 0;
				b = 0;
			}

			while ( b != 0 ) {

				if ( b & 1 ) {
					uint32_t old_low = low_res;
					low_res += a_low;
					high_res += a_high;
					
					if ( low_res < old_low )
						high_res++;
									
				}

				a_high = ( a_high << 1 ) | ( a_low >> 31 );
				a_low <<= 1;
        			b >>= 1;
			}

			if ( sign ) {
			    low_res = ~low_res;
			    high_res = ~high_res;

			    low_res++;

			    if ( low_res == 0 )
			        high_res++;
			}
			break;
		}
		
		case OPCODE::JNZ:
			result = ( RF & 0x4 ) >> 2;
			flag = 0;
			break;
	
		case OPCODE::HALT:
			halted = true;
			break;
				
	}

}

void cpu::writeback()
{
	switch ( opcode ) {
	
		case OPCODE::LOAD:			
		case OPCODE::MOV:
		case OPCODE::ADD:
		case OPCODE::SUB:
		case OPCODE::ADC:
			R [ op1 ] = result;
			PC += 6;
			break;

		case OPCODE::MUL:
			R [ op1 ] = low_res;
			R [ ++op1 ] = high_res;
			PC += 6;
			break;
		
		case OPCODE::JNZ:
			if ( !result ) {
				PC = op2;
			} else PC += 6;
			break;
	}
	RF = flag;
}


void cpu::step()
{
	fetch ();
	decode();
	execute();
	writeback();

}



void cpu::run()
{
	int i = 0;

	while ( !halted ) {
		i++;
		step();

		std::cout << "\n========== Шаг " << i << " ==========\n";

		std::cout << "PC: " << std::dec << PC << '\n';

		std::cout << "IR: 0x"
			<< std::hex << std::uppercase
			<< IR << '\n';

		std::cout << "Регистры:\n";

		for ( int j = 0; j < 8; j++ ) {
			std::cout << "R" << j + 1 << ": " 
				<< std::dec << static_cast < int32_t > ( R[j] ) << '\n';
		}

       		std::cout << "RF: 0b";

        	for ( int j = 7; j >= 0; j-- )
			std::cout << ( ( RF >> j ) & 1 );

		std::cout << "\nHALTED: " << ( halted ? "true" : "false" ) << '\n';

        	std::cout << "============================\n";
	
	}
}
