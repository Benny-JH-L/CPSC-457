/*
Vectors in C++
Vector container - dynamically allocated arrays (resizable). 
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char ** argv) {
    vector<int> v;
    for (int i=1; i<=20; i++)
        v.push_back(i);
    
    // loop through a vector
    for(auto j : v)
        cout << j << " ";

    // vectors can be indexed
    cout << "8th number is: " << v[7] << endl;

    // their size is easy to find
    cout << endl << "Vector Size = " << v.size() << '\n';

    // There are a lot of functions that can be used on vectors
    cout << "\nReverse the vector\n";
    reverse(v.begin(), v.end());
    for (auto j : v)
        cout << j << " ";
    cout << endl;

    // new items can be easily added
    v.push_back(165);
    return 0;
}