#include <iostream>

using namespace std;

// create template for swap function 
// What is T? 
// What is *?
// What is the return type of this function?
template <class T> void swap_template (T & a, T & b) {
    T c = a;
    a = b;
    b = c;
}

int main() {
    int a = 1; 
    int b = 2;
//    string a = "Hello";
//    string b = "World";

    cout << "a = " << a << " , b = " << b << endl;
    swap_template(a, b);
    cout << "a = " << a << " , b = " << b << endl;
    cout << endl;

    return 0;   
}