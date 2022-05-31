//Based on: https://www.geeksforgeeks.org/queue-linked-list-implementation/


#include <pthread.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;

pthread_mutex_t lock;
pthread_cond_t cond1;

// TODO: 1.check about the note (void*)
// 2. check if the assume is right
 
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
    Queue() {
        front = rear = NULL;
    }

    Queue createQ () {
        Queue q;
        return q;
    }
    
    void destroyQ() {
        pthread_mutex_lock(&lock);
        while (front != NULL) {
            QNode* temp = front;
            front = front->next;
            cout << temp->data << endl;
            delete(temp);
        }
        pthread_mutex_unlock(&lock);

    }
 
    void enQ(void* x) {
        pthread_mutex_lock(&lock);
        // string *sp = static_cast<string*>(x);

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
        
        pthread_mutex_unlock(&lock);
    }
 
    // Function to remove
    // a key from given queue q
    void deQ()
    {
        pthread_mutex_lock(&lock);

        // If queue is empty, return NULL.
        if (front == NULL)
            pthread_cond_wait(&cond1, &lock);
 	    else {
 		    pthread_cond_signal(&cond1); //wake up the thread
 		}
        // Store previous front and
        // move front one node ahead
        QNode* temp = front;
        front = front->next;
 
        // If front becomes NULL, then
        // change rear also as NULL
        if (front == NULL)
            rear = NULL;
 
        delete (temp);
        pthread_mutex_unlock(&lock);
    }
};

// Active Object - ex. 2

struct active_object {
    void* func1;
    void* func2;
    Queue* queue;
};

pthread_t tmp_thread;


active_object* newAO (Queue* q, void (*f1)(QNode*), void (*f2)(QNode*)) {
    
    //TODO: create a new thread ?
    
    tmp_thread = pthread_self();
    active_object* ao = new active_object();
    ao->func1 = (void*) f1;
    ao->func2 = (void*) f2;
    ao->queue = q;
    f1(ao->queue->front);
    f2(ao->queue->front);
    ao->queue->deQ();
    return ao;

}

void destroyAO (active_object* ao) {
    ao->queue->destroyQ(); //clear the Queue
    pthread_cancel(tmp_thread); //kill the thread
    pthread_exit(NULL); //terminate the program
}
