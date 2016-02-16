#include "filesystem.h"
#include "io/iosystem.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <cassert>

const int BLOCK_SIZE=64;
const int FILE_NAME_MAX_SIZE=4;
FileSystem::FileSystem() {
	io = IO_system::CreateIOSystem();

	// load the bitmask pieces
	bitmask_descriptors = 1;
	bitmask_file_blocks = 0;
	count_open_files = 0;
}

void FileSystem::createFile(std::string pname) {
	assert(pname.length() <= FILE_NAME_MAX_SIZE);
	dirent_t directory_entry;
	fd_t file_descriptor;
	char *name = (char*)pname.c_str();


	// From the bitmask, get the location 	
	int fd = getFreeSlot(bitmask_descriptors, num_descriptors);
	assert(fd != -1);

	// lock it down, and create directory entry
	strcpy(directory_entry.name, name);
	directory_entry.fd = fd - 1;
	bitmask_descriptors |= 1 << fd;
	entries[fd - 1] = directory_entry;

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
	file_entries[fd - 1] = file_descriptor;
}

void FileSystem::openFile(std::string fname) {
	// get file descriptor
	int dir;
	unsigned char buffer[64];
	fd_t f = getFd(fname, dir);

	// create entry
	oftent_t entry;
	entry.fd = entries[dir - 1].fd;
	entry.pos = 0;
	entry.length = f.length;
	io->read_block(f.block1, buffer);
	memcpy(entry.block, buffer, BLOCK_SIZE);
	open_file_table[count_open_files++] = entry;
}

void FileSystem::closeFile(int oft_index) {
	// get oft entry
	assert(oft_index < count_open_files);
	oftent_t f = open_file_table[oft_index];
	int block_num;

	// coalesce oft entries.
	for (int i = oft_index, j = oft_index + 1; j < num_descriptors; i++,j++) {
		open_file_table[i] = open_file_table[j];
	}

	count_open_files--;
}

void FileSystem::destroyFile(std::string pname) {
	assert(pname.length() <= FILE_NAME_MAX_SIZE);

	// find the directory entry
	int pos = 0;
	fd_t file = getFd(pname, pos);

	// update mask and container
	bitmask_descriptors -= (1 << pos);
	char null = '\0';
	strcpy(entries[pos - 1].name, &null);
	entries[pos - 1].fd = -1;

	// delete data from the disk.
}

void FileSystem::writeFile(int index, char c, int count) {
	assert(index < count_open_files);
	oftent_t *file = &(open_file_table[index]);
	int written = 0;
	unsigned char to_write[BLOCK_SIZE];
	while (written < count) {
		bool advance_block = !((written + (file->length % BLOCK_SIZE)) % BLOCK_SIZE);
		if (written > 0 && advance_block) {
			// write the current block to disk
			int block_num = getCurrentContentBlock(*file);
			writeToDisk(block_num, BLOCK_SIZE, (unsigned char *)file->block);

			// get the next block number
			if (block_num == file_entries[file->fd].block1) {
				block_num = file_entries[file->fd].block2;
			} else if (block_num == file_entries[file->fd].block2) {
				block_num = file_entries[file->fd].block3;
			} else {
				// refuse continued writting. 
				break;
			}
			block_num = getCurrentContentBlock(*file);
			
			// get the next block.
			unsigned char buffer[BLOCK_SIZE];
			io->read_block(block_num + 1, buffer);
			memcpy(file->block, buffer, BLOCK_SIZE);

			// continue writting.
			file->pos = 0;
		}
		file->block[file->pos] = c;
		file->pos++;
		written++;
	}

	// write the block to disk.
	file->length += written;
	int block_num = getCurrentContentBlock(*file);
	memcpy(to_write, file->block, file->pos);
	writeToDisk(block_num, file->pos, to_write);
}

FileSystem::dirent_t FileSystem::getFileDirent(std::string pname, int &i) {
	// find the directory entry
	for (int j = 0; j < num_descriptors - 1; j++) {
		if (!std::strcmp(entries[j].name, pname.c_str())) {
			i = j + 1;
			return entries[j];
		}
	}
	dirent_t nil;
	nil.fd = -1;
	return nil;
}

FileSystem::fd_t FileSystem::getFd(dirent_t dir) {
	assert(dir.fd != -1);
	return file_entries[dir.fd];
}

FileSystem::fd_t FileSystem::getFd(std::string pname, int &i) {
	return getFd(getFileDirent(pname, i));
}

int FileSystem::getFreeSlot(int bitmask, int max) {
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

int FileSystem::getFileBlock(int b) {
	return b + K + 1;
}

IO_system* FileSystem::getIO() {
	return io;
}

void FileSystem::writeToDisk(int num, int length, unsigned char data[]) {
	char nil='\0';
	length = length > BLOCK_SIZE ? length % BLOCK_SIZE : length;
	memcpy(&(data[length]), &nil, BLOCK_SIZE - length);
	io->write_block(num, data);
}

int FileSystem::getCurrentContentBlock(oftent_t f) {
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