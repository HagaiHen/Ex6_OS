#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#define MAX_NUM 64
#define ZERO 0

struct reactor {
    
    void (*funcs[MAX_NUM])(void*);
    pthread_t t;
    pthread_mutex_t lock;
    struct pollfd _pfd[MAX_NUM];
    int fd_count = 0;
    int func_count = 0;  

};

void* runReactor(void* r) {
    cout << "start run reactor" << endl;
    reactor* react = (reactor*) r;
    while (1) {
        int poll_count = poll(react->_pfd, react->fd_count, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }
        cout << "poll_count " << poll_count << endl;
        for (int i = 0; i < react->fd_count; i++) {
            if (react->_pfd[i].revents & POLLIN) {
                cout << "enter2" << endl;
                react->funcs[i](&(react->_pfd[i].fd));
            }
        }
    }    
}

void* newReactor() {

    reactor* r = new reactor();
    return r;

}

void* InstallHandler(reactor* r, void (*f)(void*), int fd) {
    if (r->fd_count <= ZERO || r->func_count <= ZERO) {
        r->_pfd[r->fd_count].fd = fd;
        r->funcs[r->func_count] = f;
        r->_pfd->events = POLLIN;
        r->fd_count++;
        r->func_count++;
        pthread_create(&(r->t), NULL, runReactor, r);
    } else {
        cout << "ADD" << endl;
        r->_pfd[r->fd_count].fd = fd;
        r->funcs[r->func_count] = f;
        r->_pfd->events = POLLIN;
        r->fd_count++;
        r->func_count++;
    }

    cout << "end install handler" << endl;
}

void* RemoveHandler(reactor* r, int fd) {
    for (int i =0; i < r->fd_count; i++) {
        if (r->_pfd[i].fd == fd) {
            r->_pfd[i].fd = -1;
            r->_pfd[i].events = ZERO;
            r->funcs[i] = NULL;
            r->fd_count--;
            r->func_count--;
        }
    }
    close(fd);
}