#include <iostream>
#include <string>

#ifndef TLB_H
#define TLB_H
class Itlb {
public:
	virtual int get_frame_cache(int sp) = 0;
	virtual void set_frame_cache(int sp, int f) = 0;
	virtual bool has_frame_cache(int sp) = 0;
	virtual std::string get_hit_string() = 0;
	virtual std::string get_miss_string() = 0;
};

class tlb : public Itlb {
public:
	tlb();
	int get_frame_cache(int sp);
	void set_frame_cache(int sp, int f);
	bool has_frame_cache(int sp);
	std::string get_hit_string();
	std::string get_miss_string();
	const std::string CLASS_TAG = "tlb::";
private:
	void lower_priorities(int lower_bound);
	int find_cached_object(int sp);
	struct buffer_obj {
		int sp;
		int f;
		int p;
	};
	static const int BUFFER_SIZE = 4;
	static const int MAX_PRIORITY = 3;
	buffer_obj buffer[BUFFER_SIZE];
};
#endif