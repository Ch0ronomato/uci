#include <iostream>
#include <cmath>
#include "pm.h"
/**
 * Example of core logic: 
 * 1. Initialize PM
 * 2. Read in VA and translate to PA.
 * 
 * Ex: 2 2048 <- Segment 2 starts at address 2048
 *	   0 2 512 1 2 -1 <- Page 0 of segment 2 starts at 512
 * Ex (cont): 0 0 0 1048576 1 1048586 1 1049088 
 * 		first tuple: 0 0: Read (first zero) from virtual address 0. w is 0, p is 0 and s 0. However,
 *		since segment 0 is not initalized, we print err.
 *
 *		Second tuple: 0 1048576: Read (first zero) from virtual address 1048576. s = 2, p = 0, w = 0
 *		bin(1048576) = 1 0000 0000 0000 0000 0000
 *                     s  p           w
 *		This this correlates to our 0 2 512 command. We read 512 and add w giving us 512
 *
 *		Third tuple: 1 1048586: Write (first 1) to virtual address 1048586. s = 2, p = 0, w = 10.
 *		So now it gets interesting. We access the disk at segment 2, page 0. Since page 0 segment 2
 *		starts at address 512, we add w, which is ten. Giving us 522. We write this then print it out.
 *
 *		Fourth tuple: 1 1049088: Write (first 1) to virutal address 1049088. s = 2, p = 1, w = 0.
 *		bin(1049088) = 1 0000 0000 0010 0000 0000
 *                     s  p           w
 *		So we attempt to write to the virtual address 1049088 and we find out that segment 2, page 1
 *		is set to -1 (1 2 -1 above). This results in a page fault. print pf.
 */

pm::pm() {
	for (int i = 0; i < (FRAME_SIZE * FRAME_COUNT); i++) {
		disk[i] = 0;
	}

	for (int i = 0; i < 32; i++) {
		bitmask[i] = 0;
	}
}

bool pm::get_physical_address(int virtual_address, pm::va_t *address) {
	va_t pa;
	pa.s = get_segment_table(virtual_address);
	pa.p = get_page_table(virtual_address);
	pa.w = get_offset(virtual_address);
	pa.addr = disk[disk[pa.s] + pa.p] + pa.w;

	if (disk[pa.s] == -1) return false;
	if (disk[disk[pa.s] + pa.p] == -1) return false;
	else {
		*address = pa; 
		return true;
	}
}

int pm::get_segment_table(int virtual_address) { // returns s
	// segement table are the 9 leading bits of the argument.
	// The total length an address can be is 28 (4 bits unused).
	int mask = ((512 - 1) << 19);
	return (virtual_address & mask) >> 19;
}

int pm::get_page_table(int virtual_address) { // returns p
	// page table are the 10 middle bits of the argument after the segement table.
	// The total length an address can be is 28 (4 bits unused).
	int mask = ((1024 - 1) << 9);
	return (virtual_address & mask) >> 9;
}

int pm::get_offset(int virtual_address)  { // returns w
	// offset are the 9 least significant bits of the argument.
	// The total length an address can be is 28 (4 bits unused).
	int mask = 512 - 1;
	return (virtual_address & mask);
}

int pm::get_free_frame(int size) {
	// just starting from 0 to 31, find an open bit.
	size = (1 << size) - 1;
	for (int i = 0; i < 32; i++) {
		if (bitmask[i] < ((1l << 32) - 1)) {
			for (int j = 0; j < 32; j++) {
				// find the open position
				if (!(bitmask[i] & (size<<j))) {
					// the jth bit is free, so frame j is free.
					return j + (i * 32);
				}
			}
		}
	}
	return -1;
}

void pm::set_frame(int frame) {
	// just starting from 0 to 31, find the current integer.
	for (int i = 0; i < 32; i++) {
		if (bitmask[i] < ((1l << 32) - 1)) {
			bitmask[i] |= (1 << frame);
			break;
		}
	}
}