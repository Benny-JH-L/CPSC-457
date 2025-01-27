#include <algorithm>
#include <iostream>
#include <list>
int main() {
	std::list<int> my_list = { 12, 5, 10, 9 };

	auto it = std::next(my_list.begin());
	std::cout<<*it<<std::endl; // this will give us second element of the list

	// end() method of list points where next pointer of the last element,
	// i.e it points to some garbage value where nect of end element is pointing

	auto garbage = my_list.end();
	std::cout<<"not actual end: "<<*garbage<<std::endl; // Some garbage value

	// to access the actual last value of a list
	// always do std::prev(<list>.end()) 

	auto actual_end = std::prev(my_list.end());
	std::cout<<"actual end: "<<*actual_end<<std::endl; // this will print 9 at the terminal

	std::cout <<"list values:"<<std::endl;
	for (int x : my_list) {
		std::cout << x << '\n';
	}
}