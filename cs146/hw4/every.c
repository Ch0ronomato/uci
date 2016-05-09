#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

// params
//	container class for the parameters of the program.
//	There is a segment, count and file. Each file is 
//	read count lines for each segment.
typedef char* string;
typedef struct params_s {
	int segment;
	int count;
	string* files;
} params_t;

params_t params_ctor(int N, int M, string files) {
	params_t t;
	t.segment = N;
	t.count = M;
	t.files = &files;
	return t;
}

// use getopts to get M,N and the file name.
// for the running programming. If there is no
// m and n argument
params_t get_parameters(int argc, string* argv) {
	char c;
	int M = 0, N = 0, use_env = 1, buffer = 0, *cur;
	cur = &N;
	while ((c = getopt(argc, argv, "1234567890,")) != -1) {
		use_env = 0;
		switch (c) {
			case '1': case '2': case '3': case '4': case'5':
			case '6': case '7': case '8': case '9': case'0':
				*cur = (*cur * 10) + (c - '0');
				break;

			case ',':
				if (cur != &M && cur != &buffer) {
					cur = &M;
				} else if (cur != &buffer) {
					cur = &buffer;
				}
				break;
		}
	}
	return params_ctor(N, M, NULL);
}

// copy_string_array
//	utility method to copy string arrays.
void copy_string_array(const int copy_length, string* dest, const string* src) {
	int i;
	for (i = 0; i < copy_length; i++) {
		dest[i] = malloc(sizeof(char) * strlen(src[i]));
		strcpy(dest[i], src[i]);
	}
}

void copy_string_arguments(const int copy_length, string* dest, ...) {
	int i;
	va_list vl;
	va_start(vl, dest);
	for (i = 0; i < copy_length; i++) {
		string elem = va_arg(vl, char*);
		dest[i] = malloc(sizeof(char) * strlen(elem));
		strcpy(dest[i], elem);
	}
	va_end(vl);
}

int get_file_names(int argc, const string *argv, string *files) {
    int i, count = 0;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            files[count] = malloc(sizeof(char) * strlen(argv[i]));
            strcpy(files[count++], argv[i]);
        }     
    }
    return count;
}

int main(int argc, string* argv) {
	// check too see what argument we need to use
    int number_of_files = 0;
	params_t args;
	string envarg = "", files[argc];
	if (argc >= 2 && argv[1][0] == '-') {
		args = get_parameters(argc, argv);
	} else if ((envarg = getenv("EVERY")) != NULL) {
		string pargs[2];
		copy_string_arguments(2, pargs, argv[0], envarg);
		args = get_parameters(2, pargs);	
	} else {
		args.segment = 1;
		args.count = 1;
	}
    number_of_files = get_file_names(argc, argv, files);
    if (!number_of_files) {
        // read from stdin
    } else {
        
    }
	return 0;
}
