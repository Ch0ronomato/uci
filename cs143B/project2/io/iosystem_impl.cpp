// Ian schweer
// 22514022
#include <iostream>
#include "iosystem.h"
#include "iosystem_impl.h"

int IO_system_impl::get_ith_block(int i) {
	return i * BLOCK_SIZE;
}

void IO_system_impl::read_block(int i, unsigned char *p) {
	// skew our index to the correct distance.
	int block = get_ith_block(i);
	for (int i = 0; i < BLOCK_SIZE; i++) {
		*(p + i) = ldisk[block + i];
	}
}

void IO_system_impl::write_block(int i, unsigned char *p) {
	// skew our index to the correct distance.
	int block = get_ith_block(i);
	for (int i = 0; i < BLOCK_SIZE; i++) {
		ldisk[block + i] = *(p + i);
	}
}

IO_system *IO_system::CreateIOSystem() {
	return new IO_system_impl();
}