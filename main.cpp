#include "queue.hpp"
#include <iostream>

//for testing
void f1 (QNode* n) {
    n->data += 1;
}

void f2 (QNode* n) {
    n->data += 2;
}
//end

int main() {
 
    Queue q;
    q.enQ("10");
    q.enQ("20");
    q.deQ();
    q.deQ();
    q.enQ("30");
    q.enQ("40");
    q.enQ("50");
    q.deQ();




    cout << "Queue Front : " << (q.front)->data << endl;
    cout << "Queue Rear : " << (q.rear)->data << endl;
    active_object ao = newAO(&q, f1, f2);
    cout << "Queue Front : " << (q.front)->data << endl;
    cout << "Queue Rear : " << (q.rear)->data << endl;
    q.destroyQ();
}
