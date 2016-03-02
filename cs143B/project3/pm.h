#ifndef PM_H
#define PM_H
const static int FRAME_SIZE = 512;
const static int FRAME_COUNT = 1024;
typedef frame_t int[FRAME_SIZE];
typedef disk_t int[FRAME_SIZE * FRAME_COUNT];
class pm 
{
public:
    typedef struct VA {
        int s; // The segment position
        int p; // The page table position
        int w; // The offset in the page table.
        int addr; // disk[disk[s] + p] + w
    } va_t;

    va_t get_virtual_address(int physical_address);
    disk_t disk;
    int bitmask[32];
private:
    int get_segment_table(int segment); // return s
    int get_page_table(int s); // returns p
    int get_offset(int p); // returns w
    int 
};
#endif
