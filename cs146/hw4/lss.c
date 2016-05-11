// Ian schweer
// 22514022
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

typedef char* string;
typedef struct node_s {
    string name;
    struct dirent* dp;
    struct stat statbuf;
} node_t;

string readlink_name(string name) {
    string buf = malloc(sizeof(char) * 256);
    if (readlink(name, buf, 256) == -1) {
        sprintf(buf, "reading link %s", buf);
        perror(buf);
    }
    sprintf(buf, "%s pointed too by %s", buf, name);
    return buf;
}

int compnode(const void *a, const void *b) {
    int diff = (*(node_t*)a).statbuf.st_size - (*(node_t*)b).statbuf.st_size;
    if (!diff) return 0;
    else if (diff > 0) return -1;
    else return 1;
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
			putchar('d');
		else
			putchar('-');

		int j = 0;
		for (j = 0; j < modecount; j++)
			if (data[i].statbuf.st_mode & modes[j])
				putchar(outs[j % 3]);
			else
				putchar('-');
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

node_t *getfiles(char *path, node_t *files, int *j) {
    struct dirent *dp;
    struct stat statbuf;
    DIR *_dir = opendir(path);
    int i = 0, dircount = 0, k = 0;
    char *dirs[BUFSIZ];
    long pathsize = 0;
    while (path[i++] != '\0') {}
    if (i > 3 && path[i - 2] != '/') {
        path[i - 1] = '/';
        path[i] = '\0';
    }
    i = 0;
    // Get all the files.
    while ((dp = readdir(_dir)) != NULL) {
       char name[BUFSIZ];
       strcpy(name, dp->d_name);
       int lstat_return = lstat(name, &files[i].statbuf);
       int stat_return = stat(name, &files[i].statbuf);
       if (lstat_return != -1 && stat_return == -1) {
            // the symbolic link points to nothing.
            perror(readlink_name(name));
            continue;
       }
       files[i].name = dp->d_name;
       files[i++].dp = dp;
    }
    *j = i;
    closedir(_dir);
}

int main(int argc, string* argv) {
    node_t nodes[BUFSIZ];
    int i = 0, size = 0;
    getfiles(".", nodes, &i);
    qsort(nodes, i, sizeof(node_t), compnode);
    iterateDirectory(nodes, i);
    return 0;
}

