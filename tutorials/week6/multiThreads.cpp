#include <iostream>
#include <thread>
#define NUM_THREADS 5

using namespace std;

void worker(int tNum) {
    cout << "Hello, I am thread #" << tNum << endl;
    return;
}

int main(int argc, char *argv[]){

    thread thread_pool[NUM_THREADS];
    cout << "Creating threads..." << endl;

    int count = 0;
    while(count<2){
        count++;


        for (int i = 0; i < NUM_THREADS; i++) {
            thread_pool[i] = thread(worker, i);
        }

        for (int i = 0; i < NUM_THREADS; i++) {
            if (thread_pool[i].joinable()) thread_pool[i].join();
        }
      
    }
    return 0;
}