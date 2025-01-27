#include <iostream>
#include <unistd.h>        
#include <sys/types.h>   
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define BUFFER_SIZE 1024

int main() {

    string input;
    cin >> input;

    const char* pathname = input.c_str();

    int fd = open(pathname, O_RDONLY);

    cout<<fd<<endl;

    char buffer[BUFFER_SIZE];

    if(read(fd, buffer, BUFFER_SIZE) > 0)
    {
        cout<<buffer<<endl;
    }
    else
    {
        cout<<"Nothing is read "<<endl;
    }
    
    close(fd);    

    return 0;
}