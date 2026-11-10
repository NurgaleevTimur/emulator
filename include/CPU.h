#ifndef CPU_H
#define CPU_H

#include "mem.h"
#include <cstdint>

enum class OPCODE : uint8_t {
	LOAD,
	MOV,
	ADD,
	SUB,
	ADC,
	MUL,
	JNZ,
	HALT
};


enum class TYPE : uint8_t {
	REG,
	IMMEDIATE,
	DIRECT,
	INDIRECT
};

class cpu
{
	private:
		uint32_t R[8] = {};
		uint8_t  RF = 0;
		uint16_t PC = 0;
		uint64_t IR = 0;
		
		OPCODE opcode;
		TYPE type;
		uint8_t op1 = 0;
		uint32_t op2 = 0;
		
		uint32_t in1;
		uint32_t in2;
		uint32_t result;
		uint32_t low_res;
		uint32_t high_res;

		uint8_t flag;

		bool halted = false;

		mem& memory;

		void fetch();
		void decode();
		void execute();
		void writeback();

	public:
		cpu ( mem& ram );
		
		void reset();
		void step();
		void run();
};

#endif // CPU_H