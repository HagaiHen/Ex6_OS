//Based on: https://www.geeksforgeeks.org/queue-linked-list-implementation/


#include <pthread.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;

 
struct QNode {
    void* data;
    QNode* next;
    
    QNode(void* d) {
        data = d;
        next = NULL;
    }
};

struct Queue {
    QNode *front, *rear;
    int* fd;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int size = 0;

    Queue() {
        front = rear = NULL;
        if (pthread_mutex_init(&(this->lock), NULL) != 0) {
            perror("mutex");
        }
        this->cond = PTHREAD_COND_INITIALIZER;
    }

    Queue* createQ () {
        Queue* q = new Queue();
        return q;
    }
    
    void destroyQ() {
        pthread_mutex_lock(&(this->lock));
        while (front != NULL) {
            QNode* temp = front;
            front = front->next;
            cout << temp->data << endl;
            delete(temp);
        }
        pthread_mutex_unlock(&(this->lock));

    }
 
    void enQ(void* x) {

        pthread_mutex_lock(&(this->lock));
        this->size++;

        // Create a new LL node
        QNode* temp = new QNode(x);
        // If queue is empty, then
        // new node is front and rear both
        if (rear == NULL) {
            front = rear = temp;
        }
 
        // Add the new node at
        // the end of queue and change rear
        rear->next = temp;
        rear = temp;
        
        pthread_cond_signal(&(this->cond)); //wake up the thread
        pthread_mutex_unlock(&(this->lock));
    }

    
 
    // Function to remove
    // a key from given queue q
    void deQ()
    {
        pthread_mutex_lock(&(this->lock));
        
        // If queue is empty, return NULL.
        if (this->size == 0) {
            // cout << "cond" << endl;
            rear = NULL;
            pthread_cond_wait(&(this->cond), &(this->lock));
        }
        // Store previous front and
        // move front one node ahead
        this->size--;
        QNode* temp = front;
        front = front->next;
        
        pthread_mutex_unlock(&(this->lock));
    }

    void set_fd(int new_fd) {
        pthread_mutex_lock(&(this->lock));
        int* pfd = (int*) malloc (sizeof(int));
        *pfd = new_fd;
        this->fd = pfd;
        pthread_mutex_unlock(&(this->lock));
    }

    int* get_fd() {
        return this->fd;
    }
};

// Active Object - ex. 2

struct active_object {
    pthread_t t;
    void (*func1)(void*);
    void (*func2)(void*);
    Queue* queue;
};


void* runAO (void* vao) {
    
    active_object* ao = (active_object*)vao;
    // cout << "start run " << (char*) ao->queue->front->data <<  endl;
    while (true) {
        if (ao->queue->size > 0) {
            char* tmp = (char*) malloc (sizeof(char));
            tmp = (char*) ao->queue->front->data;        
            ao->queue->deQ();

            if (ao->func1 != NULL) {
                ao->func1(tmp);
            }

            if (ao->func2 != NULL) {
                ao->func2(tmp);
            }
        }

    }
    // cout << "end run " << endl;
    
}


active_object* newAO (Queue* q, void (*f1)(void*), void (*f2)(void*)) {

    // cout << "enter" << endl;
    active_object* ao = new active_object();
    ao->func1 = f1;
    ao->func2 = f2;
    ao->queue = q;
    pthread_create(&(ao->t), NULL, runAO, (void*)ao);
    // cout << "end" << endl;
    return ao;

}

void destroyAO (active_object* ao) {
    ao->queue->destroyQ(); //clear the Queue
    pthread_cancel(ao->t); //kill the thread
    pthread_exit(NULL); //terminate the program
}
