#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

using namespace std;

int main()
{
    DIR * dir_stream;

    struct dirent *dstruct;

    dir_stream = opendir("..");

    if(dir_stream == NULL)
    {
        perror("open directory");

        return -1;
    }

    while(true)
    {
        dstruct = readdir(dir_stream);

        if(dstruct == NULL)
        {
            break;
        }

        cout<<"Name of the entity"<<dstruct->d_name <<endl;
    }

    closedir(dir_stream);


    return 0;
}
