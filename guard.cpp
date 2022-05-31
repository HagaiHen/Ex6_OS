#include <pthread.h>

class Guard {
    private:
        pthread_mutex_t* lock;
    
    public:
        Guard(pthread_mutex_t* lock) {
            this->lock = lock;
            pthread_mutex_lock(this->lock);
        }

        ~Guard() {
            pthread_mutex_unlock(this->lock);
        }
};



// **** Answer ****
// In a multithreaded environment, this is dangerous because many threads
// may be calling strtok with different strings.
// It will only remember the last one and return the wrong result.

