#include <iostream>
#include <array>
#include <cstdint>
#include <cstdio>
#include "iosystem.h"

#ifndef IO_SYSTEM_IMPL_H
#define IO_SYSTEM_IMPL_H

class IO_system_impl : public IO_system {
public:
	/**
	 * Ready 8 bytes from index i, and stick them
	 * in the char pointer.
	 */
	void read_block(int i, unsigned char *p);

	/**
	 * Writes the char *p to an 
	 */
	void write_block(int i, unsigned char *p);
private:
	/**
	 * Number of bytes in a block
	 */
	const int BLOCK_SIZE = 64;

	/**
	 * ldisk is really a crap ton of bytes. 
	 *
	 * There is a BIG distinction between bytes
	 * and blocks. A block is __64__ *BYTES*
	 */
	unsigned char ldisk[64 * 64];

	/**
	 * get ith block
	 *
	 * Convience method. Recieves the i block (i * 8)
	 */
	int get_ith_block(int i);
};
#endif