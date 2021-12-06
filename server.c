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

// linked list to keep information about active sockets
struct active_socket {
	int sockfd;
	char username[16];
	struct active_socket *next;
};

struct active_socket *head = NULL;

void sendMessageToClients(char message[1042]) {
	// Sends message to all clients
	
	struct active_socket *current = head;
	while(current != NULL) {
		int retval = write(current->sockfd, message, 1042);

		if (retval == -1) {
			perror("Error sending a message to user");
		}

		current = current->next;
	}

}

void removeActiveSocket(int sockfd) {

	struct active_socket *current = head;
	struct active_socket *previous = NULL;

	while(current != NULL) {

		if(current->sockfd == sockfd) {
			// when deleting first element
			if(previous == NULL) {
				head = current->next;
			}
			else {
				previous->next = current->next;
			}
			return;
		}

		previous = current;
		current = current->next;

	}

}

int main(int argc, char* argv[]) {
	struct sockaddr_in address;

	int initial_sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (initial_sockfd == 0) {
		perror("Error creating socket");
		exit(-1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( atoi(argv[1]) );

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

	int new_active_socket;

	while (true) {

		// iterating over all the linked list elements to read from each one
		struct active_socket * current = head;
		while(current != NULL) {

			char buffer[1024] = {0};
			int retval = read(current->sockfd, buffer, sizeof(buffer));
			if (retval == -1 && errno == EINVAL) {
				perror("error reading");
			}
			else if (retval == 1024) {

				// handling commands
				char buffer_to_parse[1024];
				sprintf(buffer_to_parse, "%s", buffer);
				char *token = strtok(buffer_to_parse, " ");
				if (strcmp(token, "name") == 0) {

					char username_buffer[1024];
					sprintf(username_buffer, "%s", buffer);
					memmove(username_buffer, username_buffer+5, sizeof(username_buffer));
					char messageToSend[1042];
					sprintf(messageToSend, "%s has changed their name to %s", current->username, username_buffer);
					sendMessageToClients(messageToSend);
					sprintf(current->username, "%s", username_buffer);

				}
				else {

					char messageToSend[1042];
					sprintf(messageToSend, "%s: %s", current->username, buffer);
					sendMessageToClients(messageToSend);

				}
			}
			else if (retval == 0) {

				char messageToSend[1042];
				sprintf(messageToSend, "%s has quit", current->username);
				sendMessageToClients(messageToSend);

				removeActiveSocket(current->sockfd);
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
			struct active_socket *new_socket = (struct active_socket*) malloc(sizeof(struct active_socket));
			new_socket->sockfd = new_sockfd;
			sprintf(new_socket->username, "%s%d", "User", new_sockfd);
			new_socket->next = head;
			head = new_socket;

			char user_connected_message[1042];
			sprintf(user_connected_message, "%s has connected", head->username);
			sendMessageToClients(user_connected_message);
		}
		
		sleep(1);
	}

	close(initial_sockfd);
	return 0;
}
