#include <iostream>
#include <thread>

using namespace std;

int count = 7;

void worker(int tNum) {

    printf("I am new thread\nMy number is %d\n",tNum);

    count += tNum;
    
    return;

}

int main(int argc, char *argv[]){
    // Code

    thread t1;
    
    t1 = thread(worker, 100);

    t1.join(); 
    // core will be aborted if this line is commented as main will finish before t1 
    // in most of the cases
   
    cout<<"hello, main ended\n"<<endl;

    return 0;

}