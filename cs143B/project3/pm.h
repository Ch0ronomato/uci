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
    } va_t;

    pm();
    bool get_physical_address(int physical_address, pm::va_t *pa);
    disk_t disk;
    unsigned int bitmask[32];
// private:
    int get_segment_table(int physical_address); // return s
    int get_page_table(int physical_address); // returns p
    int get_offset(int physical_address); // returns w
    int get_free_frame(int size = 1);
    void set_frame(int frame);
};
#endif
