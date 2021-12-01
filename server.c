#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <netinet/in.h>

#define PORT 2222 

//#define PATH "/student/mskroba/course/csci3500/"
#define PATH "/tmp/test3"

int main() {
	struct sockaddr_in address;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
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


	int socket_data = accept(sockfd, NULL, NULL);
	if (socket_data == -1) {
		perror("Error accepting a client");
	}

	// while (true) {
	// 	char buffer [1024];
	// 	read(socket_data, buffer, sizeof(buffer));
	// 	buffer[sizeof(buffer) - 1] = 0;
	// 	if (strncmp(buffer, "quit", sizeof(buffer))) {
	// 		break;
	// 	} 
	// 	else {
	// 		printf("%s\n", buffer);
	// 	}

	// 	close(socket_data);
	// 	int socket_data = accept(sockfd, NULL, NULL);
	// 	if (socket_data == -1) {
	// 		perror("Error accepting a client");
	// 	}
	// }

	close(sockfd);
	unlink(PATH);
	return 0;
}
