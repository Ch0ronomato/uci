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
int read_response_stream(int, int, char*, char*, char*, int);
void read_whole_stream(int, int, rio_t*, char*);
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
	int client_fd = appdat->client_fd;
	char buffer[MAXLINE], request[MAXLINE], response[MAXLINE], hostname[MAXLINE], path[MAXLINE], log[MAXLINE];
	char *request_uri, *host_address_ptr, *rest;
	int size = -1, port;
	rio_t rio;
	struct sockaddr_in clientaddr = appdat->clientaddr;
	struct hostent *host_ptr;
	
	// output the ip address of the client to our log file.
	// Luckily this isn't concurrent, so we can open a file reference
	// here.
	host_ptr = Gethostbyaddr((char *)&clientaddr.sin_addr.s_addr,
			   sizeof(clientaddr.sin_addr.s_addr), 
			   AF_INET);
	host_address_ptr = inet_ntoa(clientaddr.sin_addr);
	
	Rio_readinitb(&rio, client_fd);
	read_whole_stream(MAXLINE, client_fd, &rio, request);
	
	// We don't handle POST, PUT, or DELETE requests.
	if (strncmp(request, "GET ", strlen("GET "))) {
		printf("process_request: Received non-GET request\n");
		close(client_fd);
		request[0] = '\0';
		return;
	}
	printf("REQUEST:\n---------------------------------- \n%s\n----------------------------------\n", request);
	// filter out the "GET " from the request
	request_uri = request + 4;
	size = get_uri_size(request_uri);
	if (!ensure_get_version(&request_uri[size + 1])) {
		printf("Not proper GET version");
		return;
	}
	rest = &request_uri[size + 1]  + strlen("HTTP/1.0\r\n");
	// we have a good url, lets parse it and get our end data.
	if (parse_uri(request_uri, hostname, path, &port) < 0) {
		printf("Failed to parse url %s", request_uri);
		return;
	}
	else {
		int n = read_response_stream(client_fd, MAXLINE, hostname, path, rest, port);
		format_log_entry(log, &clientaddr, request_uri, n);
		
		// log
		FILE *logf = Fopen(LOG, "a");
		fprintf(logf, "%s %d\n", log, n);
		fflush(logf);
		fclose(logf);
	}
	//close connection
	close(appdat->client_fd);
	request[0] = '\0';
	response[0] = '\0';
	hostname[0] = '\0';
	path[0] = '\0';
	return;
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

void read_whole_stream(int size, int stream, rio_t *rio, char *response) {
	char buffer[size];
	while (rio_readlineb(rio,buffer,size) > 0) {
		// copy.
		strcat(response, buffer);
		if (strcmp(buffer, "\r\n") == 0)
			break;
	}
}

int read_response_stream(int client_f, int size, char *hostname, char *path, char *rest, int port) {
	rio_t rio;
    int response_len, n, server_fd;
	char buffer[size];
	
	server_fd = open_clientfd(hostname, port);		
	rio_writen(server_fd, "GET /", strlen("GET /"));
	rio_writen(server_fd, path, strlen(path));
	rio_writen(server_fd, " HTTP/1.0\r\n", strlen(" HTTP/1.0\r\n"));
	rio_writen(server_fd, rest, strlen(rest));
	fflush(stdout);
	Rio_readinitb(&rio, server_fd);
    while( (n = rio_readn(server_fd, buffer, size)) > 0 ) {
		response_len += n;
		printf("Recieved: %d\n", n);
		fflush(stdout);
		rio_writen(client_f, buffer, n);
		bzero(buffer, MAXLINE);
    }
	close(server_fd);
	return response_len;
}

void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, 
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /* 
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s", time_str, a, b, c, d, uri);
}
