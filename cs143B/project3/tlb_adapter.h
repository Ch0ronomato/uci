#include "tlb.h"
#include <string>
#ifndef TLB_ADAPTER_H
#define TLB_ADAPTER_H
class TlbFactory {
public:
	static Itlb *MakeAdapter(bool enabled);
private:
	class tlb_adapter : public Itlb {
	public:
		tlb_adapter(bool _enabled);
		~tlb_adapter();
		int get_frame_cache(int sp);
		void set_frame_cache(int sp, int f);
		bool has_frame_cache(int sp);
		std::string get_miss_string();
		std::string get_hit_string();
	private:
		bool enabled;
		Itlb *_tlb;
	};
};
#endif