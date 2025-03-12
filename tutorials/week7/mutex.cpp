#include <thread>
#include <unistd.h>
#include <iostream>
#include<mutex>

#define NUM_THREADS 2

// Define the mutex as a global variable to make it accessible from every one
std::mutex mutex_lock;
int balance = 0;
  
void worker(int n){
    
    for (int i=0; i<10; i++){

        mutex_lock.lock(); // Try to lock the mutex
    
        balance += n;
        std::cout<<balance<<" value inside thread"<<std::endl;
       
        mutex_lock.unlock(); 
    }
    
} 
  
int main(){

    std::thread thread_pool[NUM_THREADS];
    long changes[] = {1, -1};

    for(long i = 0; i < NUM_THREADS; i++){
        
        thread_pool[i] = std::thread(worker, changes[i]);

    }

    // Block our main thread until all threads finish their tasks
    for (int i = 0; i < NUM_THREADS; i++) {
        if (thread_pool[i].joinable()) thread_pool[i].join();
    }

    std::cout <<"final value "<< balance << std::endl;
 
} 