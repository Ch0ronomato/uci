#include <vector>
#include <string>
#include <fstream>
#include "tlb_adapter.h"
#ifndef PM_H
#define PM_H
const static int FRAME_SIZE = 512;
const static int FRAME_COUNT = 1024;
typedef int frame_t[FRAME_SIZE] ;
typedef int disk_t[FRAME_SIZE * FRAME_COUNT];
class pm 
{
public:
    typedef struct VA {
        int s; // The segment position
        int p; // The page table position
        int w; // The offset in the page table.
        int addr; // disk[disk[s] + p] + w
        int sp; // s & p together
    } va_t;

    pm(std::string outs, bool tlb_enabled = false);
    bool get_physical_address(int physical_address, pm::va_t *pa);
    void initialize(std::vector < std::vector < std::string > > data );
    void read(int virtual_address);
    void write(int virtual_address);
    void output(std::string s);
    const std::string CLASS_TAG = "pm::";
private:
    int get_segment_table(int virtual_address); // return s
    int get_page_table(int virtual_address); // returns p
    int get_offset(int virtual_address); // returns w
    int get_free_frame(int size = 1);
    void set_frame(int frame);
    void print_bitmap();
    disk_t disk;
    unsigned int bitmask[32];
    Itlb *tlb;
    std::ofstream out;
};
#endif
