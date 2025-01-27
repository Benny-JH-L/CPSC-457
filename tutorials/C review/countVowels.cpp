
//Count Vowels in string str
#include <iostream>

using namespace std;

int main() {

   // Declare Variables
   char *pt;
   int c = 0;

char str[200] = "This is cpsc457 tutorial";

pt = str;

while(*pt != '\0')
{
    if(*pt == 'a' || *pt == 'e'|| *pt == 'i' || *pt == 'o'|| *pt == 'u')
    {
        c++;
    }
    pt++;
}

cout<<"Total Vowels are: "<<c<<endl;

}

