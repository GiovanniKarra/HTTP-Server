#include "../include/http.h"


void parse_request_line(char *line, http_request *request) {
	if (strlen(line) == 0) return;

	char *token;
	while ((token = strsep(&line, " "))) {
		if (!strcmp(token, "GET")) {
			request->method = 'g';
			request->uri = (char*)malloc(sizeof(char)*MAX_FILE_PATH_SIZE);
			sprintf(request->uri, "%s", strsep(&line, " "));
			request->protocol = strsep(&line, " ");
		}
	}
}

void parse_request(int socketfd, http_request *request) {
	char buffer[MAX_REQUEST_SIZE];

	int err = read(socketfd, buffer, MAX_REQUEST_SIZE);
	if (err == -1) { perror("socket read"); exit(EXIT_FAILURE); }

	printf("%s\n", buffer);

	char *token;
	char *str = (char*)buffer;
	while ((token = strsep(&str, "\r\n"))) {
		parse_request_line(token, request);
	}
}

int uri_to_file(const char *uri, char *file) {
	char table[MAX_FILE_SIZE];

	int fd = open(URI_TABLE_FILE, O_RDONLY);
	if (fd < 0) { perror("file open"); exit(EXIT_FAILURE); }

	int err = read(fd, table, MAX_FILE_SIZE);
	if (err == -1) { perror("read file"); exit(EXIT_FAILURE); }

	int code = 404;
	strcpy(file, ERROR_FILES_LOCATION "/Error404.html");

	int first_entry_offset = 0;
	while (table[first_entry_offset] != '\n') {
		first_entry_offset++;
	}

	char *table_ptr = ((char*)table)+first_entry_offset+1;
	char *line;
	while ((line = strsep(&table_ptr, "\n"))) {
		printf("uri: %s\n", uri);
		char *token = strsep(&line, ",");
		if (strcmp(token, uri)) continue;

		strcpy(file, FILES_ROOT_LOCATION"/");
		strcat(file, strsep(&line, ","));

		char *code_str = strsep(&line, ",");
		code = atoi(code_str);

		break;
	}

	printf("file fetched: %s\n", file);

	return code;
}

char *status_code_to_message(int code) {
	switch (code)
	{
	case 404:
		return "Not Found";
		break;
	case 200:
		return "OK";
		break;
	default:
		return "Unknown error";
		break;
	}
}