
#include <iostream>
#include <unordered_map>
#include <typeinfo>
using namespace std;
  
int main()
{
    
    unordered_map<string, double> hashMap;

    hashMap["assignment1"] = 10.0;
    hashMap["assignment2"] = 20.0;
    hashMap["assignment3"] = 30.0;
    hashMap["assignment4"] = 60.0;
    hashMap["assignment5"] = 70.5;
    hashMap["assignment6"] = 90.9;
  
    
    for (auto x : hashMap){
      cout << x.first << " grade is " << x.second << endl;
    }
  
}