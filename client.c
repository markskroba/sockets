#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>

#include <netinet/in.h>
#include <arpa/inet.h>

struct socket_data {
    int sockfd;
};

void* reading_thread_entry(void* args) {
    // Gets data from the server, prints it to clients

    struct socket_data* data = args;
    int sockfd = data->sockfd;

    while (true) {

        char buffer[1042] = {0};
        int retval = read(sockfd, buffer, sizeof(buffer));
        if (retval == 0) {
            printf("Server shut down unexpectedly\n");
            close(sockfd);
            exit(0);
        }
        else if (retval == 1042) {
            printf("%s\n", buffer);
        }

        sleep(1);

    }

    return NULL;
}

void* writing_thread_entry(void* args) {
    // Gets data from the user, writes it to server
    struct socket_data* data = args;
    int sockfd = data->sockfd;

    while (true) {

        char buffer[1024] = {0};
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        write(sockfd, buffer, sizeof(buffer));

    }

    return NULL;
    
}

int main(int argc, char* argv[]) {

	int sockfd;
	struct sockaddr_in address;

    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd == 0) {
        perror("Error creating a socket");
        exit(-1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons( atoi(argv[2]) );
    address.sin_addr.s_addr = inet_addr( argv[1] );

    int retval = connect(sockfd, (struct sockaddr *) &address, sizeof(address));
    if (retval == -1 && errno != EINPROGRESS) {
        perror("Error establishing a connection to server");
        exit(-1);
    }

    // at this point, connection to server should be established

    // begin threading code
    struct socket_data data;
    data.sockfd = sockfd;

    pthread_t reading_thread_tid, writing_thread_tid;
    retval = pthread_create(&reading_thread_tid, NULL, reading_thread_entry, &data);
    retval = pthread_create(&writing_thread_tid, NULL, writing_thread_entry, &data);
    // handle possible errors

    pthread_join(reading_thread_tid, NULL);
    pthread_join(writing_thread_tid, NULL);

    return 0;
}
