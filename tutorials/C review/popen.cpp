#include <iostream>
#include <cstdio>

using namespace std;

int main()
{
    FILE* fp;
    char buffer[1000];

    //Code

    fp = popen("ls", "r");

    while(fgets(buffer, 1000, fp))
    {
        printf("%s", buffer);
    }

    pclose(fp);

       return 0;
}