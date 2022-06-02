/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/signal.h>
#include <features.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stddef.h>
#include "queue.hpp"

#define PORT "3490"
#define BACKLOG 10
#define MAXDATASIZE 1024 // max number of bytes we can get at once
Queue q;


//9980

Queue *q1 = new Queue();
Queue *q2 = new Queue();
Queue *q3 = new Queue();


void caesar_cypher (void* data) {

    char ch;
    int key = 1;
    char* msg = (char*) data;

    for(int i = 0; msg[i] != '\0'; ++i) {
         ch = msg[i];

         //encrypt for lowercase letter
         if (ch >= 'a' && ch <= 'z'){
            ch = ch + key;
            if (ch > 'z') {
               ch = ch - 'z' + 'a' - 1;
            }  
            msg[i] = ch;
         }
         //encrypt for uppercase letter
         else if (ch >= 'A' && ch <= 'Z'){
            ch = ch + key;
            if (ch > 'Z'){
               ch = ch - 'Z' + 'A' - 1;
            }
            msg[i] = ch;
         }
      }
}

void convert (void* data) {
    char ch;
    int key = 1;
    char* msg = (char*) data;
    for (int i = 0; msg[i] != '\0'; ++i) {
        ch = msg[i];  
        //convert for lowercase letter
        if (ch >= 'a' && ch <= 'z'){
            ch = (ch - 'a') + 'A';
            msg[i] = ch;
        }
        //convert for uppercase letter
        else if (ch >= 'A' && ch <= 'Z') {
                ch = (ch - 'A') + 'a';
                msg[i] = ch;
        }
    }
    

}

void enQ_2(void* x) {
    q2->enQ(x);
}

void enQ_3(void* x) {
    q3->enQ(x);
}

void send_to(void* x) {
    char* data = (char*)x;
    if (send(*q3->get_fd(), data, strlen(data), 0) < 0) {
        perror("sending");
        exit(1);
    }
}

static void * handle_client(void *new_fd) {
    
    int* ptr = (int*) new_fd;
    int sock = *ptr;
    char str[MAXDATASIZE];
    char get[MAXDATASIZE];
    active_object* ao1 = nullptr, *ao2, *ao3;    
    while (1) {
        recv(sock,str,MAXDATASIZE,0);
        q1->enQ(str);
        q3->set_fd(sock);
        if (!(strcmp(str, "q\n"))) {
            break;
        }
        if (!ao1) {
            ao1 = newAO(q1, caesar_cypher, enQ_2);
            ao2 = newAO(q2, convert, enQ_3);
            ao3 = newAO(q3, send_to, NULL);
        }
    }
    destroyAO(ao1);
    destroyAO(ao2);
    destroyAO(ao3);
    
    return NULL;
}

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

int main(void) {
    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_t thread_id;
        int * ptr = (int*) malloc (sizeof(int));
        *ptr = new_fd;
        if (pthread_create(&thread_id, NULL, handle_client, ptr) != 0) { 
            perror("start thread");
            close(new_fd);
        }
        
    }

    return 0;
}