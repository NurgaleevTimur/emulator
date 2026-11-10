#include "mem.h"
#include <cstring>

mem::mem()
{
	memset ( data, 0, sizeof ( data ) );
}

void mem::readBytes ( uint16_t address, uint8_t *buffer, size_t count )
{
	if ( sizeof ( data ) >= ( address + count ) ) {
		memcpy ( buffer, &data [ address ], count );
	}
}

void mem::writeBytes ( uint16_t address, const uint8_t *buffer, size_t count )
{
	if ( sizeof ( data ) >= ( address + count ) ) {
		memcpy ( &data [ address ], buffer, count );
	}
}

void mem::clear()
{
    memset ( data, 0, sizeof ( data ) );
}