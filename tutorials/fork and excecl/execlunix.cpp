#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() 
{
   
    cout << "Process 1 begins" << endl;

    // path to the program ls is "/bin/ls"
    // use 'which' command to locate linux commands. Try: which cat
    execl("/bin/ls", "ls", (char*)NULL);


    cout << "Process 1 ends" << endl;

    return 0;
}
