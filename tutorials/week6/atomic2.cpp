#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

int lockN = 0;                  // Secured by mutex
std::atomic<int> atomN(0);      // Secured by atomic

std::mutex mtx;

void lockSum(){
    for(int i = 0; i < 100000000; i++){

        mtx.lock();
        lockN++;
        mtx.unlock();
    }

}

void atomSum(){

    for(int i = 0; i < 100000000; i++) atomN++;

}

int main(void)
{
	auto startAtom = std::chrono:: high_resolution_clock::now();
	std::thread th1(atomSum);
    th1.join();
	auto endAtom = std::chrono:: high_resolution_clock::now();
    auto elapsedAtom = std::chrono::duration_cast<std::chrono::milliseconds>(endAtom - startAtom);

    auto startLock = std::chrono:: high_resolution_clock::now();
	std::thread th2(lockSum);
    th2.join();
	auto endLock = std::chrono:: high_resolution_clock::now();
    auto elapsedLock = std::chrono::duration_cast<std::chrono::milliseconds>(endLock - startLock);


    std::cout << "Atomic N value: " << atomN << " with execution time " << elapsedAtom.count() << " millseconds." << std::endl;
    std::cout << "Mutex  N value: " << lockN << " with execution time " << elapsedLock.count() << " millseconds." << std::endl;

	return 0;
}