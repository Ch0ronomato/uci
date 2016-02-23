// Ian schweer
// 22514022
#include "filesystem.h"
#include "filesystem_impl.h"
#include "../io/iosystem.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <vector>
#include <fstream>

const int BLOCK_SIZE=64;
const int FILE_NAME_MAX_SIZE=4;
File_system_impl::File_system_impl() {
	// load the bitmask pieces
	bitmask_descriptors = 1;
	bitmask_file_blocks = 7;
	count_open_files = 1;
}

void File_system_impl::create_file(std::string pname) {
	fd_t file_descriptor;
	char *name = (char*)pname.c_str();
	// does this file exist?
	std::vector<std::string> files = get_all_file_names_in_dir();
	if (find(files.begin(), files.end(), pname) != files.end()) {
		std::cout << "error";
		return;
	}

	// From the bitmask, get the location 	
	int fd = getFreeSlot(bitmask_descriptors, num_descriptors);
	assert(fd != -1);

	// lock it down, and create directory entry
	bool new_descriptor = fd > bitmask_descriptors;
	bitmask_descriptors |= 1 << fd;
	write_new_file(name, fd - 1, new_descriptor);

	// set the file descriptor
	// @todo: Clean this up.
	int b = getFreeSlot(bitmask_file_blocks);
	file_descriptor.length = 0;
	file_descriptor.block1 = getFileBlock(b);
	bitmask_file_blocks |= 1 << b;
	b = getFreeSlot(bitmask_file_blocks);
	file_descriptor.block2 = getFileBlock(b);
	bitmask_file_blocks |= 1 << b;
	b = getFreeSlot(bitmask_file_blocks);
	file_descriptor.block3 = getFileBlock(b);
	bitmask_file_blocks |= 1 << b;
	file_entries[fd] = file_descriptor;
	write_file_descriptor(file_descriptor, fd);

	std::cout << pname << " created";
}

void File_system_impl::open_file(std::string fname) {
	// get file descriptor
	int fd, pos;
	unsigned char buffer[64];
	fd_t f = getFd(fname, fd);

	if (f.length == -1) {
		std::cout << "error";
		return;
	}

	// ensure the file isn't open
	for (int i = 1; i < count_open_files; i++) {
		fd_t *f_o = &(file_entries[open_file_table[i].fd]);
		if (f_o->block1 == f.block1) {
			std::cout << "error";
			return;
		}
	}

	// create entry
	oftent_t entry;
	entry.fd = fd + 1;
	entry.pos = 0;
	entry.length = f.length;
	io->read_block(f.block1, buffer);
	memcpy(entry.block, buffer, BLOCK_SIZE);
	if (open_file_table[1].fd == -1) {
		pos = 1;
	} else if (open_file_table[2].fd == -1) {
		pos = 2;
	} else if (open_file_table[3].fd == -1) {
		pos = 3;
	} else {
		std::cout << "error";
	}
	count_open_files++;
	open_file_table[pos] = entry;
	std::cout << fname << " opened " << pos; // << "[length=" << f.length << "]" << std::endl;
}

bool File_system_impl::close_file_at_pos(int oft_index) {
		// get oft entry
	if (oft_index >= count_open_files) {
		return false;
	}
	if (oft_index == 0) { // don't let anyone close the directory
		return false;
	}
	oftent_t *f = &(open_file_table[oft_index]);
	int block_num, descriptor_loc;
	char buf[BLOCK_SIZE];

	count_open_files--;

	// save the contents of the buffer.
	if (!floor(f->length / BLOCK_SIZE)) {
		block_num = file_entries[f->fd].block1;
	} else if (!floor(f->length / (2*BLOCK_SIZE))) {
		block_num = file_entries[f->fd].block2;
	} else {
		block_num = file_entries[f->fd].block3;
	}
	writeToDisk(block_num, f->length % BLOCK_SIZE, (unsigned char*)f->block);

	// update the length of the file.
	file_entries[f->fd].length = f->length;
	descriptor_loc = read_block_containing_file(f->fd - 1, buf);
	for (int i = 0; i < 4; i++)
		buf[descriptor_loc + i] = (unsigned char)(f->length >> (8 * (i%4)) & 0xFF);
	writeToDisk(get_file_descriptor_block_num(f->fd), BLOCK_SIZE, (unsigned char*)buf);

	// close
	f->fd = -1;
	return true;
}
void File_system_impl::close_file(int oft_index) {
	// get oft entry
	if (close_file_at_pos(oft_index)) {
		std::cout << oft_index << " closed";
	} else {
		std::cout << "error";
	}
}

void File_system_impl::destroy_file(std::string pname) {

	// find the directory entry
	int pos = 0;
	fd_t file = getFd(pname, pos);

	// ensure the file exists
	if (file.length == -1) {
		std::cout << "error";
		return;
	}

	// ensure the file isn't open
	for (int i = 1; i < count_open_files; i++) {
		fd_t *f_o = &(file_entries[open_file_table[i].fd]);
		if (f_o->block1 == file.block1) {
			close_file_at_pos(i);
			break;
		}
	}

	// update mask and container
	bitmask_descriptors -= (1 << (pos + 1));

	// delete data from the disk.
	unsigned char buffer[64];
	writeToDisk(file.block1, 0, buffer);
	writeToDisk(file.block2, 0, buffer);
	writeToDisk(file.block3, 0, buffer);

	// update the directory.
	char buf[64];
	int descriptor = pos % BLOCK_SIZE, new_fd = -1;
	oftent_t *dir_oft = &(open_file_table[0]);
	fd_t dir_file = file_entries[0];
	int block = !floor(dir_oft->length / BLOCK_SIZE) ? dir_file.block1 : !floor(dir_oft->length / (BLOCK_SIZE * 2)) ? dir_file.block2 : dir_file.block3;
	io->read_block(block, (unsigned char*)buf);
	for (int i = 4; i < 8; i++)
		buf[(descriptor * 8) + i] = (new_fd >> (8 * (i%4)) & 0xFF);
	io->write_block(block, (unsigned char*)buf);
	dir_oft->length -= sizeof(dirent_t);

	std::cout << pname << " deleted";
}

void File_system_impl::write_file(int index, char c, int count) {
	if (index >= count_open_files) {
		std::cout << "error";
		return;
	}
	oftent_t *file = &(open_file_table[index]);
	int written = 0, prev_pos = file->pos % BLOCK_SIZE, total_count = count;
	int current_block = file->pos < BLOCK_SIZE ? 1 : (file->pos < (BLOCK_SIZE * 2) ? 2 : 3);
	unsigned char to_write[BLOCK_SIZE];

	if (file->pos == file->length && file->length == BLOCK_SIZE*3) {
		std::cout << "0 bytes written";
		return;
	}
	while (written < count) {
		bool advance_block = !(file->pos % BLOCK_SIZE);
		if (written > 0 && advance_block) {
			// write the current block to disk
			int block_num = current_block == 1 ? file_entries[file->fd].block1 : (current_block == 2 ? file_entries[file->fd].block2 : file_entries[file->fd].block3);
			writeToDisk(block_num, BLOCK_SIZE, (unsigned char *)file->block);

			if (current_block == 3) {
				// refuse writting. 
				break;
			}
			block_num = current_block == 1 ? file_entries[file->fd].block2 : file_entries[file->fd].block3;
			
			// get the next block.
			unsigned char buffer[BLOCK_SIZE];
			io->read_block(block_num, buffer);
			memcpy(file->block, buffer, BLOCK_SIZE);
			current_block++;

			file->length += written;
			count -= written;
			written = 0;
			prev_pos = 0;
		}
		file->block[file->pos % BLOCK_SIZE] = c;
		written++;
		file->pos++;
	}

	// write the block to disk.
	if (file->pos > file->length)
		file->length += written;
	int block_num = current_block == 1 ? file_entries[file->fd].block1 : (current_block == 2 ? file_entries[file->fd].block2 : file_entries[file->fd].block3);
	memcpy(to_write, file->block, BLOCK_SIZE);
	writeToDisk(block_num, BLOCK_SIZE, to_write);
	if (total_count > 192) total_count = 192;
	std::cout << total_count << " bytes written";
}

void File_system_impl::read_file(int index, int count) {
	if (index >= count_open_files) {
		std::cout << "error";
		return;
	}
	oftent_t *file = &(open_file_table[index]);
	int read = 0, offset = 0;
	int current_block = file->pos < BLOCK_SIZE ? 1 : (file->pos < (BLOCK_SIZE * 2) ? 2 : 3);
	char to_read[count + 1];
	while (read < count) {
		if (read > 0 && !(file->pos % BLOCK_SIZE)) {
			// write the current block to disk
			int block_num = current_block == 1 ? file_entries[file->fd].block1 : (current_block == 2 ? file_entries[file->fd].block2 : file_entries[file->fd].block3);
			writeToDisk(block_num, BLOCK_SIZE, (unsigned char *)file->block);
			block_num = current_block == 1 ? file_entries[file->fd].block2 : file_entries[file->fd].block3;
			
			// get the next block.
			unsigned char buffer[BLOCK_SIZE];
			io->read_block(block_num, buffer);
			memcpy(file->block, buffer, BLOCK_SIZE);
			current_block++;
		}
		to_read[read] = file->block[file->pos % BLOCK_SIZE];
		read++;
		file->pos++;
	}
	to_read[count] = '\0';
	std::cout << (char*)to_read;
}

bool File_system_impl::seek_file_to_pos(int index, int start) {
	if (index > count_open_files) return false;
	open_file_table[index].pos = start;

	if (start > open_file_table[index].length) {
		return false;
	}
	int block_num;
	if (start <= BLOCK_SIZE) {
		// load block 1
		block_num = file_entries[open_file_table[index].fd].block1;
	} else if (start <= BLOCK_SIZE * 2) {
		// load block 2
		block_num = file_entries[open_file_table[index].fd].block2;
	} else {
		// load block 3
		block_num = file_entries[open_file_table[index].fd].block3;
	}

	// load the correct buffer into the open file table.
	io->read_block(block_num, (unsigned char *)open_file_table[index].block);
	return true;
}
void File_system_impl::seek_file(int index, int start) {
	if (seek_file_to_pos(index, start)) {
		std::cout << "position is " << start;
	} else {
		std::cout << "error";
	}
}

void File_system_impl::dir() {
	std::vector<std::string> files = get_all_file_names_in_dir();
	for (std::string l : files) {
		if (l == "") continue;
		std::cout << l << " ";
	}
}

std::vector<std::string> File_system_impl::get_all_file_names_in_dir() {
	std::vector<std::string> files;
	// just run through the file names and print them.
	int length = open_file_table[0].length;
	int i = 2, j = 0; // starting at 2 because descriptor 1 is the directory.
	char name[4], len[4];
	while (i < bitmask_descriptors) {
		bool has_descriptor = (bitmask_descriptors & i) >> j;
		if (has_descriptor) {
			seek_file_to_pos(0, j * 8);
			int pos = (j * 8) % BLOCK_SIZE;
			// make sure the fd is nonzero.
			name[0] = open_file_table[0].block[pos] ;
			name[1] = open_file_table[0].block[pos + 1];
			name[2] = open_file_table[0].block[pos + 2];
			name[3] = open_file_table[0].block[pos + 3];
			files.push_back(std::string(name));
		}
		i = i << 1;
		j++;
	} 
	return files;
}

void File_system_impl::save(std::string name) {
	if (name == "") std::cout << "error";
	else {
		int i = 0;
		char buf[64];
		for (int i = 0; i < 4; i++)
			buf[i] = (unsigned char)(bitmask_descriptors >> (8 * (i)) & 0xFF);
		for (int i = 0; i < 8; i++)
			buf[i+4] = (unsigned char)(bitmask_file_blocks >> (8 * (i)) & 0xFF);
		writeToDisk(0, sizeof(int)+sizeof(long), (unsigned char*)buf);

		// close all the open files.
		for (int i = 1; i < count_open_files; i++) {
			int block_num = open_file_table[i].length;
			int fd = open_file_table[i].fd;
			block_num = floor(block_num / BLOCK_SIZE) == 0 ? file_entries[fd].block1 : floor(block_num / BLOCK_SIZE) == 1 ? file_entries[fd].block2 : file_entries[fd].block3;
			writeToDisk(block_num, open_file_table[i].length % BLOCK_SIZE, (unsigned char*)open_file_table[i].block);
		}
		open_file_table[0].fd = -1; open_file_table[1].fd = -1; open_file_table[2].fd = -1; open_file_table[3].fd = -1;
		count_open_files = 1;

		std::ofstream ofs(name);
		while (i < BLOCK_SIZE) {
			unsigned char d[BLOCK_SIZE];
			io->read_block(i++, d);
			ofs.write((char*)d, BLOCK_SIZE);
		}
		ofs.flush();
		ofs.close();
		std::cout << "disk saved";
	}
}

void File_system_impl::init(std::string name, IO_system *pio) {
	io = pio;
	setIO(pio);
	if (name != "") {
		// read in the ldisk from the file.
		unsigned char data[BLOCK_SIZE];
		int block_num = 0, i = 0;
		std::ifstream ifs(name);
		if (!ifs.good()) std::cout << "error";
		else {
			while (ifs.good() && block_num < (BLOCK_SIZE * BLOCK_SIZE)) {
				data[i++] = static_cast<unsigned char>(ifs.get());
				if (i == BLOCK_SIZE) {
					// write the block 
					i = 0;
					io->write_block(block_num++, data);
				}
			}
		}
		ifs.close();

		// get our bitmaps.
		char buf[BLOCK_SIZE], mask_one[sizeof(int)], mask_two[sizeof(long)];
		io->read_block(0, (unsigned char*)buf);
		memcpy(mask_one, &(buf[0]), sizeof(int));
		memcpy(mask_two, &(buf[sizeof(int)]), sizeof(long));
		bitmask_descriptors = *(static_cast<int*>(static_cast<void*>(mask_one)));
		bitmask_file_blocks = *(static_cast<unsigned long*>(static_cast<void*>(mask_two)));

		// // test code: remove.
		// // fill the to return obj
		// int t = bitmask_descriptors, descriptor_loc = 0, i = 0;
		// char int_value[4], buf[BLOCK_SIZE];
		// while ((t >> i) > 0) {
		// 	int_value[0] = buf[descriptor_loc];
		// 	int_value[1] = buf[descriptor_loc + 1];
		// 	int_value[2] = buf[descriptor_loc + 2];
		// 	int_value[3] = buf[descriptor_loc + 3];
		// 	to_return.length = *(static_cast<int*>(static_cast<void*>(&int_value)));
		// 	descriptor_loc += 4;
		// 	int_value[0] = buf[descriptor_loc];
		// 	int_value[1] = buf[descriptor_loc + 1];
		// 	int_value[2] = buf[descriptor_loc + 2];
		// 	int_value[3] = buf[descriptor_loc + 3];
		// 	to_return.block1 = *(static_cast<int*>(static_cast<void*>(&int_value)));
		// 	descriptor_loc += 4;
		// 	int_value[0] = buf[descriptor_loc];
		// 	int_value[1] = buf[descriptor_loc + 1];
		// 	int_value[2] = buf[descriptor_loc + 2];
		// 	int_value[3] = buf[descriptor_loc + 3];
		// 	to_return.block2 = *(static_cast<int*>(static_cast<void*>(&int_value)));
		// 	descriptor_loc += 4;
		// 	int_value[0] = buf[descriptor_loc];
		// 	int_value[1] = buf[descriptor_loc + 1];
		// 	int_value[2] = buf[descriptor_loc + 2];
		// 	int_value[3] = buf[descriptor_loc + 3];
		// 	to_return.block3 = *(static_cast<int*>(static_cast<void*>(&int_value)));

		// 	std::cout << "(length, block1, block2, block3) [";
		// 	std::cout << to_return.length << ",";
		// 	std::cout << to_return.block1 << ",";
		// 	std::cout << to_return.block2 << ",";
		// 	std::cout << to_return.block3 << "]" << std::endl;
		// }
		std::cout << "disk restored";
	} else {
		std::cout << "disk initialized";
	}
}

void File_system_impl::setIO(IO_system *pio) {
	io = pio;
	
	// make the directory with 3 blocks.
	fd_t f;
	f.length = 0;
	f.block1 = getFileBlock(0);
	f.block2 = getFileBlock(1);
	f.block3 = getFileBlock(2);
	file_entries[0] = f;

	// open the directory
	oftent_t entry;
	entry.fd = 0;
	entry.pos = 0;
	entry.length = 0;
	open_file_table[0] = entry;
	for (int i = 0; i < 4; i++)
		open_file_table[i + 1].fd = -1;

	// write to disk
	unsigned char buf[64];
	io->read_block(1, buf);
	for (int i = 0; i < 4; i++)
		buf[i] = (unsigned char)(f.length >> (8 * (i%4)) & 0xFF);
	for (int i = 0; i < 4; i++)
		buf[i + 4] = (unsigned char)(f.block1 >> (8 * (i%4)) & 0xFF);
	for (int i = 0; i < 4; i++)
		buf[i + 8] = (unsigned char)(f.block2 >> (8 * (i%4)) & 0xFF);
	for (int i = 0; i < 4; i++)
		buf[i + 12] = (unsigned char)(f.block3 >> (8 * (i%4)) & 0xFF);

	io->write_block(1, buf);
	memcpy(entry.block, buf, BLOCK_SIZE);
}

IO_system* File_system_impl::getIO() {
	return io;
}

File_system_impl::dirent_t File_system_impl::getFileDirent(std::string pname, int &i) {
	// find the directory entry
	int descriptor = 0, block = 0;
	char buf[BLOCK_SIZE], int_value[sizeof(int)];
	io->read_block(file_entries[0].block1, (unsigned char*) buf);
	dirent_t to_return;
	to_return.fd = -1;
	while (descriptor < num_descriptors && block < K) {
		char name[FILE_NAME_MAX_SIZE];
		int offset = descriptor == 0 || descriptor % descriptors_per_block;
		if (!offset) {
			int block_num = block == 1 ? file_entries[0].block2 : file_entries[0].block3;
			io->read_block(block_num, (unsigned char*) buf);
			block++;
		}
		int n = (descriptor * 8) % BLOCK_SIZE;
		name[0] = buf[n];
		name[1] = buf[n+1];
		name[2] = buf[n+2];
		name[3] = buf[n+3];

		if (!std::strcmp((char*)name, pname.c_str())) {
			i = descriptor;
			memcpy(to_return.name, name, 4);
			// std::cout << " ======== to_return.name: " << to_return.name;
			name[0] = buf[n+4];
			name[1] = buf[n+5];
			name[2] = buf[n+6];	
			name[3] = buf[n+7];
			to_return.fd = *(static_cast<int*>(static_cast<void*>(name)));
			// std::cout << " ======== fd: " << to_return.fd;
			// std::cout << "Found file entry " << (char*)to_return.name << " " << to_return.fd << std::endl;
			break;
		} else {
			descriptor++;
		}
	}
	return to_return;
}

File_system_impl::fd_t File_system_impl::getFd(dirent_t dir) {
	char int_value[sizeof(int)], buf[BLOCK_SIZE];
	int descriptor_loc = read_block_containing_file(dir.fd, buf);
	// std::cout << " ===== dir.fd: " << dir.fd;
	// std::cout << " ===== descriptor_loc: " << descriptor_loc;
	fd_t to_return;
	if (dir.fd == -1) { 
		to_return.length = -1;
		return to_return;
	}

	// fill the to return obj
	int_value[0] = buf[descriptor_loc];
	int_value[1] = buf[descriptor_loc + 1];
	int_value[2] = buf[descriptor_loc + 2];
	int_value[3] = buf[descriptor_loc + 3];
	to_return.length = *(static_cast<int*>(static_cast<void*>(&int_value)));
	descriptor_loc += 4;
	int_value[0] = buf[descriptor_loc];
	int_value[1] = buf[descriptor_loc + 1];
	int_value[2] = buf[descriptor_loc + 2];
	int_value[3] = buf[descriptor_loc + 3];
	to_return.block1 = *(static_cast<int*>(static_cast<void*>(&int_value)));
	descriptor_loc += 4;
	int_value[0] = buf[descriptor_loc];
	int_value[1] = buf[descriptor_loc + 1];
	int_value[2] = buf[descriptor_loc + 2];
	int_value[3] = buf[descriptor_loc + 3];
	to_return.block2 = *(static_cast<int*>(static_cast<void*>(&int_value)));
	descriptor_loc += 4;
	int_value[0] = buf[descriptor_loc];
	int_value[1] = buf[descriptor_loc + 1];
	int_value[2] = buf[descriptor_loc + 2];
	int_value[3] = buf[descriptor_loc + 3];
	to_return.block3 = *(static_cast<int*>(static_cast<void*>(&int_value)));
	return to_return;
}

File_system_impl::fd_t File_system_impl::getFd(std::string pname, int &i) {
	return getFd(getFileDirent(pname, i));
}

int File_system_impl::getFreeSlot(int bitmask, int max) {
	int to_return = -1;
	for (int i = 0; i < max; i++) {
		bool is_free = !((bitmask & (1 << i)) >> i);
		if (is_free) {
			to_return = i;
			break;
		}
	}

	return to_return;
}

long File_system_impl::getFreeSlot(unsigned long bitmask, int max) {
	long to_return = -1;
	for (int i = 0; i < max; i++) {
		bool is_free = !((bitmask & (1 << i)) >> i);
		if (is_free) {
			to_return = i;
			break;
		}
	}

	return to_return;
}

int File_system_impl::getFileBlock(int b) {
	return b + K + 1;
}

void File_system_impl::writeToDisk(int num, int length, unsigned char data[]) {
	char nil='\0';
	length = length > BLOCK_SIZE ? length % BLOCK_SIZE : length;
	memcpy(&(data[length]), &nil, BLOCK_SIZE - length);
	io->write_block(num, data);
}

int File_system_impl::getCurrentContentBlock(oftent_t f) {
	int block_num;
	if (floor(f.length / BLOCK_SIZE) == 0) {
		block_num = file_entries[f.fd].block1;
	} else if (floor(f.length / BLOCK_SIZE) == 1) {
		block_num = file_entries[f.fd].block2;
	} else {
		block_num = file_entries[f.fd].block3;
	}
	return block_num;
}

File_system* File_system::CreateFileSystem(IO_system *io) {
	File_system *f = new File_system_impl();
	f->setIO(io);
	return f;
}

/**
 * Adds the new file to directory!
 */
void File_system_impl::write_new_file(char* name, int descriptor, bool new_descriptor) {
	char num[4], to_write[BLOCK_SIZE];

	// read the correct block.
	descriptor = descriptor % BLOCK_SIZE;
	oftent_t *dir_oft = &(open_file_table[0]);
	fd_t dir_file = file_entries[0];
	int block = !floor(dir_oft->length / BLOCK_SIZE) ? dir_file.block1 : !floor(dir_oft->length / (BLOCK_SIZE * 2)) ? dir_file.block2 : dir_file.block3;
	io->read_block(block, (unsigned char*)to_write);
	for (int i = 0; i < 4; i++)
		to_write[((descriptor * 8) % BLOCK_SIZE) + i] = (unsigned char)name[i];
	for (int i = 4; i < 8; i++)
		to_write[((descriptor * 8) % BLOCK_SIZE) + i] = (descriptor >> (8 * (i%4)) & 0xFF);
	io->write_block(block, (unsigned char*)to_write);

	if (new_descriptor)
		dir_oft->length += sizeof(dirent_t);
}

/**
 * Adds the new descriptor, which is not in the directory.
 */
void File_system_impl::write_file_descriptor(fd_t f, int fd) {
	char new_file[16];
	int inner_pos = (fd % 4) * descriptor_size;
	int block_num = get_file_descriptor_block_num(fd); // the offset: 1 for bitmap, 6 for descriptors
	int buf_pos = 0;
	unsigned char buf[64];
	io->read_block(block_num, buf);

	for (int i = 0; i < 4; i++)
		buf[inner_pos + buf_pos++] = (unsigned char)(f.length >> (8 * (i%4)) & 0xFF);
	for (int i = 0; i < 4; i++)
		buf[inner_pos + buf_pos++] = (unsigned char)(f.block1 >> (8 * (i%4)) & 0xFF);
	for (int i = 0; i < 4; i++)
		buf[inner_pos + buf_pos++] = (unsigned char)(f.block2 >> (8 * (i%4)) & 0xFF);
	for (int i = 0; i < 4; i++)
		buf[inner_pos + buf_pos++] = (unsigned char)(f.block3 >> (8 * (i%4)) & 0xFF);

	// std::cout << "Writting discriptor to disk at " << block_num << std::endl;
	io->write_block(block_num, buf);
}

int File_system_impl::get_file_descriptor_block_num(int fd) {
	return floor(fd / descriptors_per_block) + 1;
}

int File_system_impl::read_block_containing_file(int fd, char *output_buffer) {
	int block_offset = get_file_descriptor_block_num(fd);
	int descriptor_loc = ((fd + 1) % descriptors_per_block) * descriptor_size;
	io->read_block(block_offset, (unsigned char*)output_buffer);
	return descriptor_loc ;
}
