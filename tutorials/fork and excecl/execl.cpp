// Example 1: Use execl to create another process and execute hello.cpp 
#include <iostream>
#include <cstdio>
// execl comes from the unistd.h file. 
#include <unistd.h>

using namespace std;

int main(int argc, char const *argv[]) 
{
    cout<<"Process 1 begins" << endl;
    // 1st argument: path and name of file to be executed
    // 2nd argument: file name associated to the file to be executed
    execl("./hello", "hello",(char*)NULL);

    // Does this line get printed? Why or why not?
    cout<<"Process 1 ends" << endl;

    return 0;
}