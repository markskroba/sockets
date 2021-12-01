#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 2222

int main() {

	int sockfd;
	struct sockaddr_in address;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0) {
        perror("Error creating a socket");
        exit(-1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    int retval = connect(sockfd, (struct sockaddr *) &address, sizeof(address));
    if (retval == -1) {
        perror("Error establishing a connection to server");
        exit(-1);
    }

    return 0;
}
