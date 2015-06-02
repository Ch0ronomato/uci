#include "csapp.h"
#include <sys/socket.h>
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

int get_uri_size(char*);
int ensure_get_version(char*);
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
	char buffer[MAXLINE], hostname[MAXLINE], path[MAXLINE];
	char *request_uri, *host_address_ptr, *rest;
	int size = -1, port;
	struct sockaddr_in clientaddr = appdat->clientaddr;
	struct hostent *host_ptr;
	
	// output the ip address of the client to our log file.
	// Luckily this isn't concurrent, so we can open a file reference
	// here.
	host_ptr = Gethostbyaddr((char *)&clientaddr.sin_addr.s_addr,
			   sizeof(clientaddr.sin_addr.s_addr), 
			   AF_INET);
	host_address_ptr = inet_ntoa(clientaddr.sin_addr);
	
	// get any data in the packet.
	// at this point, buffer should be the request 
	// we want to process.
	fgets(buffer,MAXLINE,client_f);
	printf("Requested: %s",buffer);
	
	// We don't handle POST, PUT, or DELETE requests.
	if (strncmp(buffer, "GET ", strlen("GET "))) {
		printf("process_request: Received non-GET request\n");
		fclose(client_f);
		return;
	}
	
	// filter out the "GET " from the request
	request_uri = buffer + 4;
	size = get_uri_size(request_uri);
	if (!ensure_get_version(&request_uri[size + 1])) {
		printf("Not proper GET version");
		return;
	}
	
	rest = request_uri[size + 1];
	// we have a good url, lets parse it and get our end data.
	if (parse_uri(request_uri, hostname, path, &port) < 0) {
		printf("Failed to parse url %s", request_uri);
		return;
	}
	else {
		FILE *server_fd = fdopen(open_clientfd(hostname, port), 'a');
		fprintf(server_fd, "GET /", strlen("GET /"));
		fprintf(server_fd, path, strlen(path));
		fprintf(server_fd, " HTTP/1.0\r\n", strlen(" HTTP/1.0\r\n"));
		fprintf(server_fd, rest, strlen(rest));
	}
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

int get_uri_size(char *uri) {
	int size = 0;
	while (uri[size] != '\0' && uri[size] != ' ') size++;
	uri[size] = '\0';
	return size;
}

int ensure_get_version(char *get_version) {
	return !(strncmp(get_version, "HTTP/1.0\r\n", strlen("HTTP/1.0\r\n")) &&
	strncmp(get_version, "HTTP/1.1\r\n", strlen("HTTP/1.1\r\n")));
}

int parse_uri(char *uri, char *hostname, char *pathname, int *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) {
	hostname[0] = '\0';
	return -1;
    }
       
    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';
    
    /* Extract the port number */
    *port = 80; /* default */
    if (*hostend == ':')   
	*port = atoi(hostend + 1);
    
    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
	pathname[0] = '\0';
    }
    else {
	pathbegin++;	
	strcpy(pathname, pathbegin);
    }

    return 0;
}

int open_clientfd(char *hostname, int port) 
{
    int clientfd;
    struct hostent hostent, *hp = &hostent;
    struct hostent *temp_hp;
    struct sockaddr_in serveraddr;

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	return -1; /* check errno for cause of error */

    temp_hp = gethostbyname(hostname);
    if (temp_hp != NULL)
	hostent = *temp_hp; /* copy */
    
    /* Fill in the server's IP address and port */
    if (temp_hp == NULL)
	return -2; /* check h_errno for cause of error */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    serveraddr.sin_port = htons(port);

    /* Establish a connection with the server */
    if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
	return -1;
    return clientfd;
}