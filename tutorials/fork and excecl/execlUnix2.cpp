
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() 
{
    cout << "Process 1 begins" << endl;


    if(fork() == 0)
    {
        execl("./hello", "hello",(char*)NULL);
    }

    wait(NULL);
    cout << "Process 1 ends" << endl;

    return 0;
}
