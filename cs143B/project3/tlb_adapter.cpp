#include "tlb.h"
#include "tlb_adapter.h"
#include <string>

Itlb* TlbFactory::MakeAdapter(bool enabled) {
	return new TlbFactory::tlb_adapter(enabled);
}

TlbFactory::tlb_adapter::tlb_adapter(bool _enabled) {
	enabled = _enabled;
	_tlb = new tlb();
}

TlbFactory::tlb_adapter::~tlb_adapter() {
	delete _tlb;
}

int TlbFactory::tlb_adapter::get_frame_cache(int sp) {
	if (enabled) {
		return _tlb->get_frame_cache(sp);
	} else {
		return -1;
	}
}

void TlbFactory::tlb_adapter::set_frame_cache(int sp, int f) {
	if (enabled) {
		_tlb->set_frame_cache(sp, f);
	}
}

bool TlbFactory::tlb_adapter::has_frame_cache(int sp) {
	if (enabled) {
		return _tlb->has_frame_cache(sp);
	} else {
		return false;
	}
}

std::string TlbFactory::tlb_adapter::get_hit_string() {
	if (enabled) {
		return _tlb->get_hit_string();
	} else {
		return "";
	}
}


std::string TlbFactory::tlb_adapter::get_miss_string() {
	if (enabled) {
		return _tlb->get_miss_string();
	} else {
		return "";
	}
}