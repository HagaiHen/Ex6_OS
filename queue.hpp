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
// 2.
 
struct QNode {
    string data;
    QNode* next;
    QNode(string d) {
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
 
    void enQ(string x) {

        pthread_mutex_lock(&lock);
 
        // Create a new LL node
        QNode* temp = new QNode(x);
 
        // If queue is empty, then
        // new node is front and rear both
        if (rear == NULL) {
            front = rear = temp;
            return;
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


active_object newAO (Queue* q, void (*f1)(QNode*), void (*f2)(QNode*)) {
    tmp_thread = pthread_self();
    //QNode* tmp = q.front;

    // while(tmp != NULL) {
    //     (*f1)(tmp);
    //     tmp = tmp->next;
    // }

    // tmp = q.front;
    
    // while(tmp != NULL) {
    //     (*f2)(tmp);
    //     tmp = tmp->next;
    // }
    active_object ao;
    ao.func1 = (void*)f1;
    ao.func2 = (void*)f2;
    ao.queue = q;
    return ao;

}

void destroyAO (Queue* q, void (*f1)(QNode*), void (*f2)(QNode*)) {
    q->destroyQ(); //clear the Queue
    // pthread_cancel(tmp_thread); //cancel the thread
    // pthread_exit(NULL); //terminate
}
