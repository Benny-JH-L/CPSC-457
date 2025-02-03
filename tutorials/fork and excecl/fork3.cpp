#include <iostream>
#include <cstdio>
#include <unistd.h>

using namespace std;

int main()
{
    pid_t pid1;
    pid_t pid2;
    pid_t pid3;
    cout<<"Hello World"<<endl;

    pid1=fork();
    pid2=fork();
    pid3=fork();

    cout<<"Goodbye World "<<endl;
    return 0;
}