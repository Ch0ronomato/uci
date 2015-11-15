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

typedef struct node {
	char *name;
	int dir;
} node_t;

node_t* iterateDirectory(char *path, char *explore[], int explore_length) {
	node_t nodes[BUFSIZ];
	struct dirent *dp;
	struct stat statbuf;
	struct passwd *pwd;
	struct group *grp;
	struct tm *tm;
	char datestring[256];

	DIR *_dir = opendir(path);
	while ((dp = readdir(_dir)) != NULL) {
		if (stat(dp->d_name, &statbuf) == -1)
			continue;

		printf("%10.10s", sperm(statbuf.st_mode));
		printf("%4d", statbuf.st_nlink);

		if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
			printf(" %-8.8s", pwd->pw_name);
		else
			printf(" %-8d", statbuf.st_uid);

		if ((grp = getgrgid(statbuf.st_gid)) != NULL)
			printf(" %-8.8s", grp->gr_name);
		else
			printf(" %-8d", statbuf.st_gid);

		printf(" %9jd", (intmax_t)statbuf.st_size);
		tm = localtime(&statbuf.st_mtime);

		strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
		printf(" %s %s\n", datestring, dp->d_name);
		// if current item is directory, add to list of
		// directories to iterate.
			
		// add item into struct	
	}
	return nodes;

}

int main(int argc, char **argv) {
	// get the "current" directory.
	char *path = ".";
	char *explore[BUFSIZ];
	int ifiles = 0, explore_length = 1, i = 0, j = 0;
	if (argc == 2)
		path = argv[1];
	explore[0] = path;
	// iterate over all files in directory
	iterateDirectory(path, explore, explore_length);
	// print out directory information.
	return 0;
}

