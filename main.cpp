#include "queue.hpp"
#include <iostream>

//for testing
void f1 (QNode* n) {
    n->data = (void*) "1";
}

void f2 (QNode* n) {
    n->data = (void*) "2";
}
//end

int main() {
 
    Queue q;
    
    char s1[2] = {'1','\0'};
    char* s2 = "2";
    char* s3 = "3";

    
    
    // void* p4 = &s4;
    // void* p5 = &s5;
    
    q.enQ(s1);
    q.enQ(s2);
    q.enQ(s3);
    // q.enQ(p4);
    // q.enQ(p5);

    cout << "Queue Front : " << (char*)(q.front)->data << endl;
    cout << "Queue Rear : " << (char*)(q.rear)->data << endl;
    q.deQ();
    q.deQ();
    
    // active_object* ao = newAO(&q, f1, f2);
    // destroyAO(ao);
    cout << "Queue Front : " << (q.front)->data << endl;
    cout << "Queue Rear : " << (q.rear)->data << endl;
    // q.destroyQ();
}
