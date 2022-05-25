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
    
    string s1 = "1", s2 = "2", s3 = "3", s4 = "4", s5 = "5";

    void* p1 = &s1;
    void* p2 = &s2;
    void* p3 = &s3;
    void* p4 = &s4;
    void* p5 = &s5;

    q.enQ(p1);
    q.enQ(p2);
    q.enQ(p3);
    q.enQ(p4);
    q.enQ(p5);

    cout << "Queue Front : " << (q.front)->data << endl;
    cout << "Queue Rear : " << (q.rear)->data << endl;
    q.deQ();
    q.deQ();
    
    //active_object ao = newAO(&q, f1, f2);
    cout << "Queue Front : " << (q.front)->data << endl;
    cout << "Queue Rear : " << (q.rear)->data << endl;
    q.destroyQ();
}
