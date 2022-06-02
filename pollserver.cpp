/*
** pollserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include "reactor.hpp"
#define PORT "3490" // Port we're listening on
#define MAX_SIZE 1024

reactor* react = new reactor();

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void) {
    int listener; // Listening socket descriptor
    int yes=1; // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

// Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}



void recv_f (void* pfd) {
    cout << "recv_f" << endl;
    int* fd = (int*) pfd;
    int listener = *fd;
    char str[MAX_SIZE];
    // If not the listener, we're just a regular client
    int nbytes = recv(listener, str, strlen(str), 0);
    if (nbytes <= 0) {
    // Got error or connection closed by client
        if (nbytes == 0) {
            // Connection closed
            printf("pollserver: socket %d hung up\n", listener);
        } else {
            perror("recv");
        }
    } else {
        // We got some good data from a client
        for(int j = 0; j < react->fd_count; j++) {
        // Send to everyone!
        int dest_fd = react->_pfd[j].fd;
            // Except the listener and ourselves
            if (react->funcs[j] == recv_f && dest_fd != listener) {
                if (send(dest_fd, str, nbytes, 0) == -1) {
                    perror("send");
                }
            }
        }
    }

}

void handle_client(void* pfd) {
    int* fd = (int*) pfd;
    int listener = *fd;
    int newfd;
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char remoteIP[INET6_ADDRSTRLEN];
    cout << "before accept" << endl;
    newfd = accept(listener,(struct sockaddr *)&remoteaddr, &addrlen);
                
    if (newfd == -1) {
        perror("accept");
    } else {
        InstallHandler(react, recv_f, newfd);
        printf("pollserver: new connection from %s on socket %d\n", 
        inet_ntop(remoteaddr.ss_family,
        get_in_addr((struct sockaddr*)&remoteaddr),
        remoteIP, INET6_ADDRSTRLEN),
        newfd);
    }
}


// Main
int main() {

    int listener; // Listening socket descriptor

    // Set up and get a listening socket
    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    InstallHandler(react, handle_client, listener);
    printf("pollserver: waiting for new connections...\n"); 
    pthread_join(react->t, NULL);
    close(listener);
    return 0;
}
 
