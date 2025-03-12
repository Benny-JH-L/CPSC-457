#include <iostream>
#include <thread>
#include <mutex>

std::mutex m1;
int thread_no = 1;

void recieveGreetings(){

    m1.lock();

    std::cout<<"Thread "<<thread_no<<", Hello, I locked this mutex!"<<std::endl;
    thread_no++;

    m1.unlock();


}
int main(){
  
    std::thread thread1(recieveGreetings);
    std::thread thread2(recieveGreetings);
    std::thread thread3(recieveGreetings);

    thread1.join();
    thread2.join();
    thread3.join();

    return 0;

}