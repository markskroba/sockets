#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include <netinet/in.h>

#define PORT 2222 

int main() {
	struct sockaddr_in address;

	int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (sockfd == 0) {
		perror("Error creating socket");
		exit(-1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	int retval = bind(sockfd, (struct sockaddr *) &address, sizeof(address));
	if (retval == -1) {
		perror("Error binding a path to socket");
		exit(-1);
	}

	retval = listen(sockfd, 10);
	if (retval == -1) {
		perror("Error listening for clients");
		exit(-1);
	}	
	int active_socket;

	while (true) {

		char buffer[1024] = {0};
		int retval = read(active_socket, buffer, sizeof(buffer));
		if (retval == -1 && errno == EINVAL) {
			perror("error reading");
		}
		else if (retval != -1) {
			printf("message: '%s'\n", buffer);
			
			char buffer[1024] = {0};
			sprintf(buffer, "%s", "test message to client");
			printf("sending message to client\n");
			write(active_socket, buffer, sizeof(buffer));
			printf("mesage sent\n");

			return 0;
		}

		//adding new sockets
		int new_socket = accept4(sockfd, NULL, NULL, SOCK_NONBLOCK);
		if (new_socket == -1 && errno != EAGAIN) {
			perror("error");
		}
		else if (new_socket != -1) {
			printf("established new connection with socket %d\n", new_socket);
			active_socket = new_socket;
		}
		
		sleep(1);
	}

	close(sockfd);
	return 0;
}
