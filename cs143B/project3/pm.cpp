#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "pm.h"
#include "logger.h"
#include "tlb_adapter.h"
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

pm::pm(std::string outs, bool tlb_enabled) : out(outs) {
	for (int i = 0; i < (FRAME_SIZE * FRAME_COUNT); i++) {
		disk[i] = 0;
	}

	for (int i = 0; i < 32; i++) {
		bitmask[i] = 0;
	}

	tlb = TlbFactory::MakeAdapter(tlb_enabled);
	LoggerFactory::GetLogger()->log(CLASS_TAG + "PM()", "Constructed");
}

bool pm::get_physical_address(int virtual_address, pm::va_t *address) {
	va_t pa;
	pa.w = get_offset(virtual_address);
	pa.s = get_segment_table(virtual_address);
	pa.p = get_page_table(virtual_address);
	pa.sp = (pa.s << 10) + pa.p;
	pa.addr = disk[disk[pa.s] + pa.p] + pa.w;
	*address = pa; 
	std::string out = "{ s=" + std::to_string(pa.s) + ", p=" + std::to_string(pa.p);
	out += + ", w=" + std::to_string(pa.w) + ", addr = " + std::to_string(pa.addr) + ", sp = " + std::to_string(pa.sp) + "}";
	LoggerFactory::GetLogger()->log(CLASS_TAG + "get_physical_address()", "Getting virtual address components for " + std::to_string(virtual_address));
	LoggerFactory::GetLogger()->log(CLASS_TAG + "get_physical_address()", out);
	return true;
}

void pm::initialize(std::vector < std::vector < std::string > > data ) {
	std::string tag = CLASS_TAG + "initialize()";
	if (data.size() < 2) {
		LoggerFactory::GetLogger()->log(tag, "bad input file");
		return;
	}
	std::vector < std::string > segments = data[0];
	std::vector < std::string > pages = data[1];

	set_frame(0);
	LoggerFactory::GetLogger()->log(tag, "Set frame 0");
	for (int i = 0; i < segments.size(); i += 2) {
		int s = std::stoi(segments[i]), f = std::stoi(segments[i+1]);
		if (f > 0) {
			// make f a frame, not address
			int frame = floor(f / FRAME_SIZE);
			disk[s] = f;
			set_frame(frame);
			LoggerFactory::GetLogger()->log(tag, "Segment " + std::to_string(s) + " set frame " + std::to_string(frame) + " to address " + std::to_string(f));
			set_frame(frame + 1); // page tables take 2 entries.
			LoggerFactory::GetLogger()->log(tag, "Segment " + std::to_string(s) + " set frame " + std::to_string(frame + 1) + " to address " + std::to_string(f));
		} else {
			disk[s] = f;
			LoggerFactory::GetLogger()->log(tag, "Segment " + std::to_string(s) + " has no frame (" + std::to_string(-1) + ")");
		}
	}

	for (int i = 0; i < pages.size(); i += 3) {
		int p = std::stoi(pages[i]), s = std::stoi(pages[i+1]);
		int f = std::stoi(pages[i + 2]);
		if (f > 0) {
			disk[disk[s] + p] = f;
			LoggerFactory::GetLogger()->log(tag, "Set disk " + std::to_string(disk[s] + p) + " to " + std::to_string(f));
			set_frame(f / FRAME_SIZE);
		} else {
			disk[disk[s] + p] = f;
			LoggerFactory::GetLogger()->log(tag, "Set disk " + std::to_string(disk[s] + p) + " to " + std::to_string(f));
		}
	}

}

void pm::read(int virtual_address) {
	va_t addr;
	std::string tag = CLASS_TAG + "read()", msg = "Virtual address " + std::to_string(virtual_address) + " ";
	LoggerFactory::GetLogger()->log(tag, "Starting read for virtual address " + std::to_string(virtual_address));
	get_physical_address(virtual_address, &addr);
	if (tlb->has_frame_cache(addr.sp)) {
		output(tlb->get_hit_string());
		output(std::to_string(tlb->get_frame_cache(addr.sp) + addr.w));
		output(" ");
		LoggerFactory::GetLogger()->log(tag, msg + " recieved from tlb at addr " + std::to_string(tlb->get_frame_cache(addr.sp) + addr.w));
	}
	else {
		output(tlb->get_miss_string());
		if (addr.s == 0) {
			output("err ");
			LoggerFactory::GetLogger()->log(tag, msg + "error");
		}
		else if (disk[addr.s] == -1 || (disk[disk[addr.s] + addr.p]) == -1) {
			output("pf ");
			LoggerFactory::GetLogger()->log(tag, msg + "page fault");
		} else if (!addr.s || !disk[addr.s]|| (!disk[disk[addr.s] + addr.p])) {
			output("err ");
			LoggerFactory::GetLogger()->log(tag, msg + "error");
		} else {
			output(std::to_string(addr.addr) + " ");
			tlb->set_frame_cache(addr.sp, disk[disk[addr.s] + addr.p]);
			LoggerFactory::GetLogger()->log(tag, msg + "at address " + std::to_string(addr.addr));
		}
	}
	LoggerFactory::GetLogger()->log(tag, "Ending read for virtual address " + std::to_string(virtual_address));
}

void pm::write(int virtual_address) {
	va_t addr;
	std::string tag = CLASS_TAG + "write()", msg = "Virtual address " + std::to_string(virtual_address) + " ";
	LoggerFactory::GetLogger()->log(tag, "Starting write for virtual address " + std::to_string(virtual_address));
	get_physical_address(virtual_address, &addr);
	if (tlb->has_frame_cache(addr.sp)) {
		output(tlb->get_hit_string());
		output(std::to_string(tlb->get_frame_cache(addr.sp) + addr.w));
		output(" ");
		LoggerFactory::GetLogger()->log(tag, msg + " recieved from tlb at addr " + std::to_string(tlb->get_frame_cache(addr.sp) + addr.w));
	}
	else {
		output(tlb->get_miss_string());
		if (addr.s == 0) {
			output("err ");
			LoggerFactory::GetLogger()->log(tag, msg + "error");
		}
		else if (disk[addr.s] == -1 || (disk[disk[addr.s] + addr.p]) == -1) {
			output("pf ");
			LoggerFactory::GetLogger()->log(tag, msg + "page fault");
		}
		else {
			if (!disk[addr.s]) {
				int frame_number = get_free_frame(2);
				disk[addr.s] = frame_number * FRAME_SIZE;	
				set_frame(frame_number);
				set_frame(frame_number + 1)	;
				LoggerFactory::GetLogger()->log(tag, msg + "allocated frames (2) " + std::to_string(disk[addr.s]));
			}
			if (!disk[disk[addr.s] + addr.p]) {
				int frame_number = get_free_frame(1);
				disk[disk[addr.s] + addr.p] = frame_number * FRAME_SIZE;
				set_frame(frame_number);
				LoggerFactory::GetLogger()->log(tag, msg + "allocated frame " + std::to_string(disk[disk[addr.s] + addr.p]));
			}
			output(std::to_string(disk[disk[addr.s] + addr.p] + addr.w));
			output(" ");
			tlb->set_frame_cache(addr.sp, disk[disk[addr.s] + addr.p]);
			LoggerFactory::GetLogger()->log(tag, msg + "at address " + std::to_string(disk[disk[addr.s] + addr.p] + addr.w));
		}
	}
	LoggerFactory::GetLogger()->log(tag, "Ending write for virtual address " + std::to_string(virtual_address));
}

void pm::output(std::string s) {
	out << s;
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
	print_bitmap();
	return -1;
}

void pm::set_frame(int frame) {
	int mask = floor(frame / 32);
	frame = frame % 32;
	bitmask[mask] |= (1 << frame);
	print_bitmap();
}

void pm::print_bitmap() {
	std::string tag = CLASS_TAG + "print_bitmap()";
	for (int i = 0; i < 32 && bitmask[i]; i++) {
		LoggerFactory::GetLogger()->log(tag, "Bitmap " + std::to_string(i) + " = " + std::to_string(bitmask[i]));
	}
}
