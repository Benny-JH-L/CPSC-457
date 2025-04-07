
#include <iostream>
#include <cstdio>
#include <cmath> 

using namespace std;

typedef signed long long int64_t;

int main()
{
    int64_t page_size = 3;
    int need_size = 22;
    int64_t res = std::ceil((float)need_size / page_size);

    cout << ceil(need_size / page_size) << endl;
    cout << res << endl;

    double x = 4.3;
    double result = std::ceil(x);
    std::cout << "Ceiling of " << x << " is " << result << std::endl;
    
    return 0;
}
