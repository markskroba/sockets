#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 2222

int main() {

	int sockfd;
	struct sockaddr_in address;

    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd == 0) {
        perror("Error creating a socket");
        exit(-1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    int retval = connect(sockfd, (struct sockaddr *) &address, sizeof(address));
    if (retval == -1 && errno != EINPROGRESS) {
        perror("Error establishing a connection to server");
        exit(-1);
    }

    printf("established connection %d\n", retval);

    char buffer[1024] = {0};
    sprintf(buffer, "%s", "test");
    printf("message: %s, size: %d\n", buffer,sizeof(buffer));
    write(sockfd, buffer, sizeof(buffer));
    printf("message sent\n");

    return 0;
}
