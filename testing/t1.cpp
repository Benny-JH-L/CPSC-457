#include <iostream>
#include <vector>
#include <iterator>  // for std::prev

using namespace std;

int main() {
    std::vector<int> vec = {10, 20, 30, 40};

    // Get const_iterator to the last element
    auto last = std::prev(vec.cend());
    auto front = vec.cbegin();
    auto prev_front = prev(front);
    std::cout << "Last element: " << *last << std::endl;
    std::cout << "Front element: " << *front << std::endl;
    // if (prev_front < front && prev_front )
    //     std::cout << "Prev Front element: " << *prev_front<< std::endl;
    // else
    //     std::cout << "Prev Front element: NULL" << std::endl;
    return 0;
}