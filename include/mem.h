#ifndef MEM_H
#define MEM_H

#include <cstddef>
#include <cstdint>

class mem
{
	private:
		uint8_t data [ 4096 ];

	public:
		mem();

		void readBytes ( uint16_t address, uint8_t *buffer, size_t count );
		void writeBytes ( uint16_t address, const uint8_t *buffer, size_t count );
		void clear();
};

#endif // MEM_H