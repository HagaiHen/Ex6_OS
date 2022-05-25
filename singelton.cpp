#include <pthread.h>
#include <string>
#include <iostream>

using namespace std;
class singelton;
pthread_mutex_t lock;

singelton* single_instance = nullptr; //global val

// TODO: 1.test 2.template


class singelton {

    private:
        string data;
        singelton() {
            this->data = "hello from singelton Design Pattern";
        }
    
    public:
        singelton* Instance() {
            pthread_mutex_lock(&lock);
            if (single_instance == nullptr) { //using the constructor only in the first time
                single_instance = new singelton();
            }
            pthread_mutex_unlock(&lock);
            return single_instance;
        }

        void Destroy() {
            single_instance = nullptr;
            delete (this);
        }
};

int main () {
    singelton* s;
    s->Instance();
    s->Destroy();
    s->Instance();
    s->Destroy();
    s->Instance();
    s->Instance();
    s->Instance();
    s->Instance();
    
}