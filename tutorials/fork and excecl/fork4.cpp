#include <stdio.h>
#include <unistd.h>
#include <iostream>
using namespace std;
int main()
{
    int x =10;

    for(int i=0; i<4; i++)
    {
        fork();
    }

    x++;

    cout<<"The value of x is: "<<x<<endl;
    
}