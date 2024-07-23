#include "../include/http.h"


int main(int argc, char const *argv[])
{
	int server_fd, new_socket;
	ssize_t valread;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	while (true) {
		// printf("My address is : ");
		// uint32_t addr = address.sin_addr.s_addr;
		// for (int i = 0; i < 4; i++) {
		// 	printf("%d.", *(((char*)&addr)+i));
		// }
		// printf("\n");

		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		
		http_request request;
		http_response response;

		parse_request(new_socket, &request);
		printf("\n--- REQUEST PARSED ---\n");
		printf("RECEIVED REQUEST FOR %s\n", request.uri);
		process_request(&request, &response);
		printf("\n--- REQUEST PROCESSED ---\n");
		printf("CODE %d\n", response.status_code);
		send_response(new_socket, &response);
		printf("\n--- RESPONSE SENT ---\n\n");
	}

	// closing the connected socket
	close(new_socket);
	// closing the listening socket
	close(server_fd);
	return 0;
}
