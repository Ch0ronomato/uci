#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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
	char *name;
	struct dirent *dp;
	struct stat statbuf;
} node_t;

int cmpnode(const void *a, const void *b) {
	return strcasecmp((*(node_t *)a).name, (*(node_t *)b).name);
}

int cmpstr(const void *a, const void *b) {
	const char **s1 = (const char **)a;
	const char **s2 = (const char **)b;
	return strcasecmp(*s1, *s2);
}
void iterateDirectory(node_t *data, int length) {
	struct passwd *pwd;
	struct group *grp;
	struct tm *tm, *curr;
	char datestring[256];
	int modecount = 9;
	mode_t modes[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
	char outs[] = {'r', 'w', 'x'};
	int i;
	
	// get the current time.
	time_t rawtime;
	time(&rawtime);
	for (i = 0; i < length; i++) {
		if (S_ISDIR(data[i].statbuf.st_mode))
			printf("d");
		else
			printf("-");

		int j = 0;
		for (j = 0; j < modecount; j++)
			if (data[j].statbuf.st_mode & modes[j])
				printf("%c", outs[j % 3]);
			else
				printf("-");
		printf("%2d", data[i].statbuf.st_nlink);

		if ((pwd = getpwuid(data[i].statbuf.st_uid)) != NULL)
			printf(" %-8.8s", pwd->pw_name);
		else
			printf(" %-8d", data[i].statbuf.st_uid);

		if ((grp = getgrgid(data[i].statbuf.st_gid)) != NULL)
			printf(" %-6.8s", grp->gr_name);
		else
			printf(" %-8d", data[i].statbuf.st_gid);

		printf("%5jd ", data[i].statbuf.st_size);	
		tm = localtime(&data[i].statbuf.st_mtime);

		if (difftime(rawtime, mktime(tm)) < 15768000) { 
			strftime(datestring, sizeof(datestring), "%b %e %R", tm);
			printf("%s %s\n", datestring, data[i].dp->d_name);
		} else {
			strftime(datestring, sizeof(datestring), "%b %e %Y", tm);
			printf("%s %s\n", datestring, data[i].dp->d_name);
		}
	}
}

node_t* getfiles(char *path, char *explore[], node_t *files, int *j, long *size) {
	struct dirent *dp;
	struct stat statbuf;
	DIR *_dir = opendir(path);
	int i = 0, dircount = 0, k = 0;
	char *dirs[BUFSIZ];
	long pathsize = 0;
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
		if (stat(name, &files[i].statbuf)== -1)
			continue;
		files[i].name = dp->d_name;
		files[i].dp = dp;
		if (S_ISDIR(files[i].statbuf.st_mode)) {
			dirs[dircount] = (char *) malloc(strlen(name));
			strcpy(dirs[dircount++], name);
		}
		pathsize += (long)files[i].statbuf.st_blocks;
		i++;
	}
	qsort(dirs, dircount, sizeof(char *), cmpstr);
	qsort(files, *j, sizeof(node_t), cmpnode);
	*j = i;
	printf("%s:\ntotal %d\n", path, (pathsize / 2));
	iterateDirectory(files, i); 
	printf("\n");
	for (i = 0; i < dircount; i++) {
		getfiles(dirs[i], explore, files, &k, size); 
	}
	*size = pathsize;
	return files;
} 
int main(int argc, char **argv) {
	// get the "current" directory.
	char *path = "./";
	char *explore[BUFSIZ];
	node_t nodes[BUFSIZ], *ret;
	int ifiles = 0, explore_length = 1, i = 0, j = 0;
	long size;
	if (argc == 2)
		path = argv[1];
	explore[0] = path;
	// iterate over all files in directory
	for(; i < explore_length; i++) {
		ret = getfiles(explore[i], explore, nodes, &j, &size);
		qsort(nodes, j, sizeof(node_t), cmpnode);
	}
	// print out directory information.
	return 0;
}

