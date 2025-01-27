#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char * const argv[]) 
{
    //char str[100];
    string filename = argv[1];

    // ofstream file;
    // file.open(argv[1]);

    // char str2[] = "Tutorial 12 CPSC 457";

    // file<<str2<<endl;

    // file.close();
        
    ifstream file2;

    file2.open(argv[1]);
    char str3[100];
    file2.get(str3, 100);

    cout<<"The content of the file is: "<<str3<<endl;

    file2.close();

    return 0;
}
