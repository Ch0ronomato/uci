// Ian schweer
// 22514022
/**
 * Single flat list of all files.
 * Implemented as one regular file. Allows us to use normal operations.
 * For each sector in directory, we will have a file name and a discriptor
 *
 * ldisk [0] -> bitmask block
 * ldisk [1] -> (directory) 1 block, 64 bytes, that contain 2 integer pairs of, the name (4 chars), location (int)
 * ldisk [2...k] -> 4 descriptors / block, each
 *	* Each descriptor has 4 integers. 4 ints * 4 bytes = 16 / 64 bytes * block = 1/4 block 
 */
 #include "../io/iosystem.h"
 #include "filesystem.h"
 #include <string>
 #include <vector>
 #ifndef FILESYSTEM_IMPL_H
 #define FILESYSTEM_IMPL_H
class File_system_impl : public File_system
{
public:
	File_system_impl();
	void create_file(std::string name) override;
	void destroy_file(std::string name) override;
	void open_file(std::string name) override;
	void close_file(int oft_index) override;
	void write_file(int index, char c, int count) override;
	void read_file(int index, int count) override;
	void seek_file(int index, int start) override;
	void dir() override;
	void save(std::string name) override;
	void init(std::string name, IO_system *pio) override;
	IO_system *getIO() override;
	void setIO(IO_system *io) override;
private:

	// typedefs
	typedef struct dirent_s {
		char name[4];
		int fd;
	} dirent_t;

	typedef struct fd_s {
		int length;
		int block1;
		int block2;
		int block3;
	} fd_t;

	typedef struct oft_entry_s {
		char block[64];
		int length; 
		int pos;
		int fd;
	} oftent_t;

	// members
	int bitmask_descriptors;
	unsigned long bitmask_file_blocks;
	int count_open_files;
	const int K = 6;
	const int num_descriptors = 24;
	const int descriptors_per_block = num_descriptors / K;
	const int descriptor_size = 64 / descriptors_per_block;
	dirent_t entries[23];
	fd_t file_entries[24];
	oftent_t open_file_table[4];

	// methods
	int getFreeSlot(int bitmask, int max=32);
	long getFreeSlot(unsigned long bitmask, int max=64);
	int getFileBlock(int b);
	dirent_t getFileDirent(std::string pname, int &i);
	fd_t getFd(std::string pname, int &i);
	fd_t getFd(dirent_t);
	void writeToDisk(int num, int length, unsigned char* data);
	int getCurrentContentBlock(oftent_t f);
	void write_new_file(char* name, int descriptor, bool new_descriptor);
	void write_file_descriptor(fd_t f, int fd);
	int read_block_containing_file(int fd, char *output_buffer);
	int get_file_descriptor_block_num(int fd);
	bool seek_file_to_pos(int index, int start);
	bool close_file_at_pos(int);
	std::vector<std::string> get_all_file_names_in_dir();
};
#endif
