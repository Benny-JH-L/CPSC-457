#include <iostream>
#include <set>

// data is sorted
// no repetition allowed
 
int main()
{
  std::set<char> a;
  a.insert('A');
  a.insert('N');
  a.insert('S');
  a.insert('H');
  
  for(auto& character: a)
  {
    std::cout << character << "-";
  }
  std::cout << "\n------end---------\n";
  return 0;
}
