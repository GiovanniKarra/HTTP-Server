#ifndef SERVER_H_

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#define PORT 8080

#define MAX_REQUEST_SIZE 4096
#define MAX_FILE_PATH_SIZE 256
#define MAX_FILE_SIZE 65536

#define FILES_ROOT_LOCATION "../files" 
#define ERROR_FILES_LOCATION "../files/errors"
#define URI_TABLE_FILE "../files/uri_table.csv"

#define OK 200
#define FORBIDDEN 403
#define NOT_FOUND 404


typedef struct {
	char method;
	char *uri;
	char *protocol;
	char *hostname;
	char *body;
} http_request;

typedef struct {
	char *protocol;
	uint16_t status_code;
	char *status_message;
	char *connection;
	size_t content_size;
	char *content_type;
	char *body;
} http_response;


void parse_request(int socketfd, http_request *request);
void process_request(http_request *request, http_response *response);
void send_response(int socket, http_response *response);

int uri_to_file(const char *uri, char *file);
char *status_code_to_message(int code);

#endif