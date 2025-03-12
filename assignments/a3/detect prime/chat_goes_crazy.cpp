// /// ============================================================================
// /// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
// /// All Rights Reserved. Do not distribute this file.
// /// ============================================================================
// ///
// /// You must modify this file and then submit it for grading to D2L.
// ///
// /// You can delete all contents of this file and start from scratch if
// /// you wish, as long as you implement the detect_primes() function as
// /// defined in "detectPrimes.h".

// #include "detectPrimes.h"
// #include <cmath>
// #include <condition_variable>
// #include <cstdio>
// #include <cstdlib>
// #include <mutex>

// #include <unordered_map>
// #include <thread>
// #include <atomic>
// #include <iostream>

// using namespace std;

// // C++ barrier class (from lecture notes).
// // You do not have to use it in your implementation. If you don't use it, you
// // may delete it.
// class simple_barrier {
//     std::mutex m_;
//     std::condition_variable cv_;
//     int n_remaining_, count_;
//     bool coin_;
  
//     public: 
//     simple_barrier(int count = 1) { init(count); }
//     void init(int count)
//     {
//       count_ = count;
//       n_remaining_ = count_;
//       coin_ = false;
//     }
//     bool wait()
//     {
//       if (count_ == 1) return true;
//       std::unique_lock<std::mutex> lk(m_);
//       if (n_remaining_ == 1) {
//         coin_ = ! coin_;
//         n_remaining_ = count_;
//         cv_.notify_all();
//         return true;
//       }
//       auto old_coin = coin_;
//       n_remaining_--;
//       cv_.wait(lk, [&]() { return old_coin != coin_; });
//       return false;
//     }
//   };



// size_t nextIndex = -1;
// std::atomic<bool> global_finished = false;
// std::vector<int64_t> result;

// // used for all threads - if all threads think is true then is prime, else composite
// std::vector<bool> isPrime;
// simple_barrier barrier;
// // This function takes a list of numbers nums[] and returns a subset of them
// // that are primes. Duplicates in input must be duplicated in the output.
// //
// // The parameter n_threads indicates how many threads should be created to speed
// // up the computation.
// // -----------------------------------------------------------------------------
// // You will most likely need to re-implement this function entirely.
// // Note that the current implementation ignores n_threads. Your multithreaded
// // implementation must use this parameter.

// // check for all divisors in array of boolean?

// void thread_function(int threadIndex, int n_threads, const std::vector<int64_t>& nums){
//     while(true){
//         std::thread threads[256];
//         // true until proven false
//         isPrime[threadIndex] = true;

//         // serial task
//         // last number to get to barrier gets the next number
//         if (barrier.wait()){
//             // increment index and get current number
//             nextIndex++;
//             // no more numbers left
//             if (nextIndex >= nums.size()){
//                 // signal no more work
//                 global_finished = true;
//             }
//         }

//         barrier.wait();

//         // parallel task
//         // if all threads check and no more work, break
//         if(global_finished) {
//             break;
//         }

//         int64_t currentNum = nums[nextIndex];

//         // check int
//         // check for base cases
//         if (currentNum < 2 or currentNum % 2 == 0 or currentNum % 3 == 0){
//             // if either 2 or 3 still true
//             if (currentNum != 2 and currentNum != 3){
//                 isPrime[threadIndex] = false;

//             }                
//         }
//         else{

//             // code given from hint in tutorial
//             int64_t i = 5 + 6* threadIndex;
//             int64_t max = sqrt(currentNum);
//             while (i <= max) {
//                 if (currentNum % i == 0 || currentNum % (i + 2) == 0) isPrime[threadIndex] = false;
//                 i += 6*n_threads;
//             }
//         }
    
//         // wait for all to finish checking
//         barrier.wait();

//         // serial task to combine results
//         if (barrier.wait()){
//             bool allPrime = true;
//             for (int i = 0; i < n_threads; i++){
//                 if (!isPrime[i]){
//                     allPrime = false;
//                     break;
//                 }
//             }

//             if (allPrime){
//                result.push_back(currentNum); 
//             }
//         }

//         barrier.wait();

//     }
// }

// std::vector<int64_t> detect_primes(const std::vector<int64_t> & nums, int n_threads){

//     isPrime.resize(n_threads);
//     barrier.init(n_threads);
//     std::thread threads[256];
//     // Task tasks[256];
//     nextIndex = -1;
//     result.clear();
 

//     for (int i = 0; i < n_threads; i++){
//         threads[i] = std::thread(thread_function, i, n_threads, nums);
//     }

//     for (int i = 0; i < n_threads; i++){
//         if (threads[i].joinable()) threads[i].join();
//     }

//     return result;
// }

#include "detectPrimes.h"
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <iostream>

using namespace std;

struct Task {
    int64_t start = 0;
    int64_t end = 0;
};

mutex result_mutex;
unordered_map<int64_t, bool> resultMap;
atomic<bool> global_determined_primality = false;
vector<int64_t> global_result;

void check_prime_task(int64_t number, int64_t start, int64_t end, atomic<bool>& is_prime) {
    for (int64_t i = start; i <= end; i += 6) {
        if (number % i == 0 || number % (i + 2) == 0) {
            is_prime = false;
            return;
        }
    }
}

bool is_prime_multithreaded(int64_t number, int n_threads) {
    if (number < 2) return false;
    if (number <= 3) return true;
    if (number % 2 == 0 || number % 3 == 0) return false;

    int64_t limit = sqrt(number);
    int64_t step = (limit - 5) / 6 + 1;
    int64_t chunk_size = step / n_threads;
    int remainder = step % n_threads;

    atomic<bool> is_prime = true;
    vector<thread> threads;
    int64_t current_index = 0;

    for (int i = 0; i < n_threads; ++i) {
        int64_t start = 5 + (current_index * 6);
        int64_t end = start + (chunk_size * 6) - 1;
        if (i < remainder) end += 6;
        if (end > limit) end = limit;
        threads.emplace_back(check_prime_task, number, start, end, ref(is_prime));
        current_index += chunk_size + (i < remainder);
    }

    for (auto& t : threads) {
        t.join();
    }
    return is_prime;
}

vector<int64_t> detect_primes(const vector<int64_t>& nums, int n_threads) {
    vector<int64_t> primes;
    for (int64_t num : nums) {
        bool is_prime;
        {
            lock_guard<mutex> lock(result_mutex);
            if (resultMap.count(num)) {
                is_prime = resultMap[num];
            } else {
                is_prime = is_prime_multithreaded(num, n_threads);
                resultMap[num] = is_prime;
            }
        }
        if (is_prime) primes.push_back(num);
    }
    return primes;
}
