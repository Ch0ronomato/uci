#include "csapp.h"
#include <ifaddrs.h>
#define LOG "proxy_log.txt"

//struct designed to hold data used by multiple functions in main.c
typedef struct{
	FILE* logfile;
	int client_fd;
	int server_fd;
	struct sockaddr_in clientaddr;
} AppData;

//static struct to hold application data
static AppData *appdat;

//initializes static appdat
void init_app(){
	appdat = (AppData*)malloc(sizeof(AppData));
	appdat->logfile = fopen(LOG, "a");
}

//destructor-like function for static appdat
void destroy_app(){
	fclose(appdat->logfile);
	free(appdat);
}

/**
 * Provided functions
 */
int parse_uri(char *uri, char *target_addr, char *path, int  *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

void process_req();
void print_this_ip();

int main(int argc, char** argv) {
	
	//debug function to print ip
	print_this_ip();
	
	if (argc != 2) {
		printf("Please provide an integer for the port\n");
		return;
	}
	
	init_app();

	
	int listenfd; /* The connection file descriptor */
	int port = atoi(argv[1]);

	// lets open the file descriptor
	listenfd = open_listenfd(port);
	
	// main loop
	while (1) {
		//debug text
		printf("Now Listening...\n");
		fflush(stdout);
		
		//Accept client connections
		int clientlen = sizeof(appdat->clientaddr);
		appdat->client_fd = Accept(listenfd, (SA *)&appdat->clientaddr, (socklen_t *) &clientlen);
		
		//debug text
		printf("Connection Accepted...\n");
		fflush(stdout);
		
		//Handle client request
		process_req();
	}
}

void process_req(){
	//Prefer to work with FILE
	FILE *client_f = fdopen(appdat->client_fd,"r");
	char buffer[MAXLINE];
	
	//testing
	fgets(buffer,MAXLINE,client_f);
	printf("%s",buffer);
	
	//close connection
	fclose(client_f);
	close(appdat->client_fd);
}

//debug function to print ip
void print_this_ip(){
	struct ifaddrs *addrs;
	getifaddrs(&addrs);
	struct ifaddrs *tmp = addrs;

	while (tmp) 
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
		{
			struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
			printf("%s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
		}

		tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
}