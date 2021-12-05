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

// linked list to keep information about active sockets
struct active_socket {
	int sockfd;
	char username[16];
	struct active_socket *next;
};

struct active_socket *head = NULL;

void sendMessageToClients(char message[1042]) {
	
	struct active_socket *current = head;
	while(current != NULL) {
		int retval = write(current->sockfd, message, 1042);

		if (retval == -1) {
			perror("Error sending a message to user");
		}

		current = current->next;
	}

}

int main() {
	struct sockaddr_in address;

	int initial_sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (initial_sockfd == 0) {
		perror("Error creating socket");
		exit(-1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	int retval = bind(initial_sockfd, (struct sockaddr *) &address, sizeof(address));
	if (retval == -1) {
		perror("Error binding a path to socket");
		exit(-1);
	}

	retval = listen(initial_sockfd, 10);
	if (retval == -1) {
		perror("Error listening for clients");
		exit(-1);
	}	
	printf("server have started");
	int new_active_socket;

	while (true) {

		// iterating over all the linked list elements to read from each one
		struct active_socket * current = head;
		while(current != NULL) {

			char buffer[1024] = {0};
			int retval = read(current->sockfd, buffer, sizeof(buffer));
			printf("reading from %s\n", current->username);
			if (retval == -1 && errno == EINVAL) {
				perror("error reading");
			}
			else if (retval != -1) {
				char messageToSend[1042];
				sprintf(messageToSend, "%s: %s", current->username, buffer);
				printf("message: %s\n", messageToSend);
				sendMessageToClients(messageToSend);
			}

			current = current->next;

		}

		//adding new sockets
		int new_sockfd = accept4(initial_sockfd, NULL, NULL, SOCK_NONBLOCK);
		if (new_sockfd == -1 && errno != EAGAIN) {
			perror("error");
		}
		else if (new_sockfd != -1) {
			// Handle adding socket, add new active_socket at the beginning of a list
			printf("has connected\n");
			struct active_socket *new_socket = (struct active_socket*) malloc(sizeof(struct active_socket));
			new_socket->sockfd = new_sockfd;
			sprintf(new_socket->username, "%s%d", "User", new_sockfd);
			new_socket->next = head;
			head = new_socket;
			//TODO: Message below should be sent to all other users, not printed by server
			printf("%s has connected\n", head->username);
		}
		
		sleep(1);
	}

	close(initial_sockfd);
	return 0;
}
