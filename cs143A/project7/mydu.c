// Ian schweer
// 22514022
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <errno.h>
#include <stdint.h>

typedef struct node {
	int size;
	char *name;
} node_t;

int cmpnode(const void *a, const void *b) {
	node_t *left = (node_t *)a, *right = (node_t *)b;
	if (left->size == right->size) return strcasecmp(left->name, right->name);
	else return left->size - right->size;	
}

void iterateDirectory(node_t *data, int length) {
	int i, j = 0;
	for (i = 0; i < length; i++) {
		while(data[i].name[++j] != '\0');
		data[i].name[j - 1] = '\0';
		printf("%-7ld %s\n", data[i].size, data[i].name);
	}
		
}

int getfiles(char *path, node_t *files, int *j) {
	struct dirent *dp;
	struct stat statbuf;
	DIR *_dir = opendir(path);
	int i = 0, size = 0;
	while (path[i++] != '\0') {};
	if (path[i - 2] != '/') {
		path[i - 1] = '/';
		path[i] = '\0';
	}
	i = 0;
	while ((dp = readdir(_dir)) != NULL) {
		char name[BUFSIZ];
		strcpy(name, path);
		strcat(name, dp->d_name);
		if (dp->d_name[0] == '.') continue;
		if (lstat(name, &statbuf)== -1)
			continue;
		if (S_ISDIR(statbuf.st_mode))
			size += getfiles(name, files, j) + ((statbuf.st_blocks * 512) / 1000); 
		else size += (statbuf.st_blocks * 512) / 1000;
	}
	files[*j].name = (char*) malloc(BUFSIZ);
	strcpy(files[*j].name, path); 
	files[*j].size = size == 0 ? 4 : size + 4;
	(*j)++;
	return size;
} 
int main(int argc, char **argv) {
	// get the "current" directory.
	char *path = "./";
	node_t nodes[BUFSIZ];
	int ifiles = 0, explore_length = 1, i = 0, j = 0;
	if (argc == 2)
		path = argv[1];
	getfiles(path, nodes, &j);
	qsort(nodes, j, sizeof(node_t), cmpnode);
	iterateDirectory(nodes, j);
	// print out directory information.
	return 0;
}

