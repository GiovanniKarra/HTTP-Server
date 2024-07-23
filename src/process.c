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

	// response->body[file_size] = '\0';

	response->content_type = get_content_type(file);
	response->content_size = file_size;

	close(fd);
	free(file);
	free(request->uri);
}

void send_response(int socket, http_response *response) {
	int size_buffer_value = 128;
	int buff_size = response->content_size+strlen(response->protocol)+size_buffer_value+
					strlen(response->status_message)+strlen(response->content_type);

	// use malloc because a big file (ie image) can surpass the stack's maximum size
	char *buffer = (char*)malloc(buff_size*sizeof(char));
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

	// printf("size: %ld\n", response->content_size);
	if (response->content_size > 0) {
		int index = strlen(buffer);
		memcpy(buffer+index, response->body, response->content_size);
		memcpy(buffer+index+response->content_size, "\r\n\r\n", 5);
	}

	// size_t response_size = strlen(buffer);

	// printf("%s\n", buffer);
	
	int err = send(socket, buffer, buff_size, 0);
	if (err == -1) { perror("socket send"); exit(EXIT_FAILURE); }

	free(response->body);
	free(buffer);
	// free(response->content_type);
}
