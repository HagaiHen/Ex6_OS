#include "queue.hpp"
#include <iostream>


int main() {
 
    Queue q;
    
    char s1[2] = {'1','\0'};
    char s2[2] = {'2','\0'};
    char s3[2] = {'3','\0'};
    

    
    q.enQ(s1);
    q.enQ(s2);
    q.enQ(s3);

    cout << "Queue Front : " << (char*)(q.front)->data << endl;
    cout << "Queue Rear : " << (char*)(q.rear)->data << endl;
    q.deQ();
    q.deQ();
    
    
    cout << "Queue Front : " << (q.front)->data << endl;
    cout << "Queue Rear : " << (q.rear)->data << endl;
    
}
