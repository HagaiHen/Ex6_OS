#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_NUM 64

struct reactor {
    
    void* funcs[MAX_NUM];
    int fds[MAX_NUM];
    pthread_t t;
    pthread_mutex_t lock;
    struct pollfd* _pdf;
    int fd_count = 0;
    int func_count = 0;

    
        

};

void* runReactor(void* r) {
    reactor* react = (reactor*) r;
    while (1) {
        int poll_count = poll(react->_pdf, react->fd_count, -1);
        
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < react->fd_count; i++) {
            
        }
    }    
}

void* newReactor() {

    reactor* r = new reactor();
    return r;

}

void* InstallHandler(reactor* r, void* f, int fd) {
    if (r->fd_count > 0 || r->func_count > 0) {
        r->_pdf->fd = fd;
        r->_pdf->events = POLLIN;
        r->fd_count++;
        r->func_count++;
        pthread_create(&(r->t), NULL, runReactor, r);
    } else {
        r->_pdf->fd = fd;
        r->_pdf->events = POLLIN;
        r->fd_count++;
        r->func_count++;
    }
}