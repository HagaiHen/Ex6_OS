// Based on: https://gist.github.com/daniebker/2299755

#include <string>
#include <iostream>

using namespace std;

#ifndef _SINGLETON_H
#define _SINGLETON_H

// TODO: 1.test

pthread_mutex_t lock;

template<typename T>
class Singelton {
    public:
        static T* Instance();
        static void Destroy();

    private:

        Singelton(Singelton const&){};
        Singelton& operator=(Singelton const&){};
        static T* m_instance;
        Singelton(){ m_instance = static_cast <T*> (this); };
        ~Singelton(){  };
    };

    template<typename T>
    T* Singelton<T>::m_instance = nullptr;

    template<typename T>
    T* Singelton<T>::Instance() {
        pthread_mutex_lock(&lock);
        if(!m_instance) {

            Singelton<T>::m_instance = new T();
            cout << &m_instance << endl;
        }
        pthread_mutex_unlock(&lock);
        return m_instance;
    }

    template<typename T>
    void Singelton<T>::Destroy() {
    
        delete Singelton<T>::m_instance;
        Singelton<T>::m_instance = nullptr;
    }

#endif

int main () {
    Singelton<string>* s; 
    s->Instance();
    s->Destroy();
    s->Instance();
    s->Instance();
    s->Instance();
    s->Instance();
    s->Destroy();
    s->Instance();
}