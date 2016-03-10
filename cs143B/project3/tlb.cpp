#include <iostream>
#include <string>
#include "tlb.h"
#include "Logger.h"

const int DEFAULT = -1;

tlb::tlb() {
	// set default values.
	for (int i = 0; i < BUFFER_SIZE; i++) {
		buffer[i].sp = -1;
		buffer[i].p = i;
	}
}

int tlb::get_frame_cache(int sp) {
	std::string tag = CLASS_TAG + "get_frame_cache()";
	int f = DEFAULT, i = find_cached_object(sp);
	if (i > DEFAULT) {
		f = buffer[i].f;
		int current = buffer[i].p;
		buffer[i].p = MAX_PRIORITY + 1;
		lower_priorities(current);
		LoggerFactory::GetLogger()->log(tag, "Making buffer " + std::to_string(i) + " highest priority");
	}

	return f;
}

void tlb::set_frame_cache(int sp, int f) {
	// evict the lowest level cache
	int lowest = 0;
	for (int i = 1; i < BUFFER_SIZE; i++) {
		if (buffer[i].sp == sp) {
			// we have this item in tlb already.
			lowest = i;
			break;
		} else {
			lowest = buffer[i].p < buffer[lowest].p ? i : lowest;
		}
	}

	LoggerFactory::GetLogger()->log(CLASS_TAG + "set_frame_cache", "Evicting cache item " + std::to_string(lowest));
	int current = buffer[lowest].p;
	buffer[lowest].p = MAX_PRIORITY + 1;
	buffer[lowest].sp = sp;
	buffer[lowest].f = f;
	lower_priorities(current);
}

bool tlb::has_frame_cache(int sp) {
	return find_cached_object(sp) > -1 ? true : false;
}

void tlb::lower_priorities(int lower_bound) {
	std::string tag = CLASS_TAG + "lower_priorities()";
	for (int i = 0; i < BUFFER_SIZE; i++) {
		if (buffer[i].p > lower_bound) {
			buffer[i].p--;
		}
		std::string msg = "Making address " + std::to_string(buffer[i].sp) + "(" + std::to_string(i) + ") ";
		msg += "to priority " + std::to_string(buffer[i].p);
		LoggerFactory::GetLogger()->log(tag, msg);
	}
}

int tlb::find_cached_object(int sp) {
	int cached = DEFAULT;
	for (int i = 0; i < BUFFER_SIZE; i++) {
		if (buffer[i].sp == sp) {
			cached = i;
		}
	}
	return cached;
}

std::string tlb::get_hit_string() {
	return "h ";
}

std::string tlb::get_miss_string() {
	return "m ";
}