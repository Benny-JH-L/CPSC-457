#include <sys/types.h>

#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main()
{
    struct stat sstruct;

    const char *path = "input.txt";

    if(stat(path, &sstruct) < 0)
    {
        return 1;
    }

    cout<< "File Size:= "<<sstruct.st_size<<"bytes"<<endl;
    cout<< "uid:= "<<sstruct.st_uid<<""<<endl;
    cout<< "atime:= "<<sstruct.st_atime<<""<<endl;
}
