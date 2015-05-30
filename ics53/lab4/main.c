#include "csapp.h"
#define LOG "proxy_log.txt"

/**
 * Provided functions
 */
int parse_uri(char *uri, char *target_addr, char *path, int  *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Please provide an integer for the port\n");
		return;
	}
	int connectionfd; /* The connection file descriptor */
	int port = atoi(argv[1]);
	FILE logfile = Fopen(LOG, 'a');

	// lets open the file descriptor
	connectionfd = open_listenfd(port);
	
	// main loop
	while (1) {
		
	}
}