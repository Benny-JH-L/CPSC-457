#include <iostream>
#include <thread>

using namespace std;

void worker(int tNum) {

    printf("the thread number is : %d \n", tNum);
    return;
//    Code
}

int main(int argc, char *argv[]){
    // Code
    int counter  = 120;
    thread t1;

    if(t1.joinable())
    {
        cout<<"-----------1------------"<<endl;
    }


    t1 = thread(worker, counter);

    if(t1.joinable())
    {
        cout<<"-----------2------------"<<endl;
        t1.join();
    }

    if(t1.joinable())
    {
        cout<<"-----------3------------"<<endl;
    }

    cout<<"Bye"<<endl;
    return 0;

}