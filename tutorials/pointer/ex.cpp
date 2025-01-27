
#include <iostream>
#include <stdio.h>

using namespace std;

int main()
{
    int val = 20;
     
    cout << val << endl;

    int* ptr = &val;
    *ptr = *ptr + 1;

    cout << *ptr << endl;
    cout << val << endl;


    return 0;
}
