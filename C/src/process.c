#include "../include/http.h"


void process_request(http_request *request, http_response *response) {
	response->protocol = "HTTP/1.1";
	response->connection = "close";

	char *file = (char*)malloc(sizeof(char)*MAX_FILE_PATH_SIZE);
	response->status_code = uri_to_file(request->uri, file);
	response->status_message = status_code_to_message(response->status_code);

	int fd = open(file, O_RDONLY);

	size_t file_size = lseek(fd, 0, SEEK_END);
	if (file_size == -1) { perror("file seek"); exit(EXIT_FAILURE); }
	lseek(fd, 0, SEEK_SET);
	
	response->body = malloc(sizeof(char)*file_size+1);

	int err = read(fd, response->body, file_size);
	if (err == -1) { perror("file read"); exit(EXIT_FAILURE); }

	response->body[file_size] = '\0';

	char *type = strchr(file+2, '.');
	if (type == NULL || type == file) type = "plain";

	response->content_type = (char*)malloc(sizeof(char)*32);
	sprintf(response->content_type, "text/%s", type+1);
	response->content_size = strlen(response->body);

	close(fd);
	free(file);
	free(request->uri);
}

void send_response(int socket, http_response *response) {
	int size_buffer_value = 128;
	int buff_size = strlen(response->body)+strlen(response->protocol)+size_buffer_value+
					strlen(response->status_message)+strlen(response->content_type);

	char buffer[buff_size];
	buffer[0] = '\0';
	
	strcat(buffer, response->protocol);
	strcat(buffer, " ");

	char str_stat_code[3];
	sprintf(str_stat_code, "%d", response->status_code);
	strcat(buffer, str_stat_code);
	strcat(buffer, " ");

	strcat(buffer, response->status_message);
	strcat(buffer, "\r\n");

	if (response->content_size > 0) {
		char str_content_len[8];
		sprintf(str_content_len, "%ld", response->content_size);
		strcat(buffer, "Content-Length: ");
		strcat(buffer, str_content_len);
		strcat(buffer, "\r\n");
	}

	strcat(buffer, "Connection: ");
	strcat(buffer, response->connection);
	strcat(buffer, "\r\n");

	if (strlen(response->content_type) > 0) {
		strcat(buffer, "Content-Type: ");
		strcat(buffer, response->content_type);
		strcat(buffer, "\r\n");
	}
	strcat(buffer, "\r\n");  // mandatory 'empty line'

	if (strlen(response->body) > 0) {
		strcat(buffer, response->body);
		strcat(buffer, "\r\n\r\n");
	}

	size_t response_size = strlen(buffer);

	// printf("%s\n", buffer);
	
	int err = send(socket, buffer, response_size, 0);
	if (err == -1) { perror("socket send"); exit(EXIT_FAILURE); }

	free(response->body);
	free(response->content_type);
}
