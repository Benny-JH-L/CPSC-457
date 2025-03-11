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

// struct Task
// {
//     int64_t* num_to_check;
//     bool checkNum = false;
// };


// // C++ barrier class (from lecture notes).
// // You do not have to use it in your implementation. If you don't use it, you
// // may delete it.
// class simple_barrier 
// {
//     std::mutex m_;
//     std::condition_variable cv_;
//     volatile int n_remaining_, count_;
//     volatile bool coin_;

//   public:
//     simple_barrier(int count = 1)
//     {
//         init(count);
//     }
//     void init(int count)
//     {
//         count_ = count;
//         n_remaining_ = count_;
//         coin_ = false;
//     }
//     bool wait()
//     {
//         if (count_ == 1) { return true; }
//         std::unique_lock<std::mutex> lk(m_);
//         if (n_remaining_ == 1) {
//             coin_ = ! coin_;
//             n_remaining_ = count_;
//             cv_.notify_all();
//             return true;
//         }
//         auto old_coin = coin_;
//         n_remaining_--;
//         cv_.wait(lk, [&]() { return old_coin != coin_; });
//         return false;
//     }
// };

// // returns true if n is prime, otherwise returns false
// // -----------------------------------------------------------------------------
// // to get full credit for this assignment, you will need to adjust or even
// // re-write the code in this function to make it multithreaded.
// // static bool is_prime(int64_t n)
// // {
// //     // there are no primes <2
// //     if (n < 2) return false;
// //     // 2 and 3 are primes
// //     if (n <= 3) return true;
// //     // but multiples of 2 and 3 are not
// //     if (n % 2 == 0 or n % 3 == 0) return false;
// //     // try to divide n by every number 5 .. sqrt(n)
// //     int64_t i = 5;
// //     int64_t max = sqrt(n);
// //     while (i <= max) {
// //         if (n % i == 0 or n % (i + 2) == 0) return false;
// //         i += 6;
// //     }
// //     // didn't find any divisors, so n must be a prime
// //     return true;
// // }

// // may need to edit this...
// bool thread_is_prime(int64_t& n)
// {
//     // there are no primes <2
//     if (n < 2) return false;
//     // 2 and 3 are primes
//     if (n <= 3) return true;
//     // but multiples of 2 and 3 are not
//     if (n % 2 == 0 or n % 3 == 0) return false;
//     // try to divide n by every number 5 .. sqrt(n)
//     int64_t i = 5;
//     int64_t max = sqrt(n);
//     while (i <= max) {
//         if (n % i == 0 or n % (i + 2) == 0) return false;
//         i += 6;
//     }
//     // didn't find any divisors, so n must be a prime
//     return true;
// }

// // use barrier or mutex to add to these data structures
// simple_barrier barrier = simple_barrier();
// mutex myMutex;
// vector<int64_t> result;
// unordered_map<int64_t, bool> resultMap; // a cache, remembers previously computed numbers if they were prime.
// atomic<bool> finished = false;
// int64_t curr_index = 0;




// /*
// void thread_function(const vector<int64_t>& nums, int thread_id, Task& task)
// {
//     while(true)
//     {
//         // pick one thread with the barrier
//         if (barrier.wait())
//         {
//             // debug
//             cout << "Chosen: thread #" << thread_id << endl;    
//             cout << "Curr index = " << curr_index << endl;  

//             // check if we've gone through all the numbers in `num`
//             if (curr_index >= nums.size())
//             {
//                 cout << "\tGone through all the numbers, exiting..." << endl;    // debug
//                 finished = true;
//                 break;
//             }
//             *task.num_to_check = nums[curr_index];              // set this thread to compute if this number is prime
//             curr_index++;
//             task.checkNum = true;
//         }

//         // synchronize again before starting parallel work
//         barrier.wait();

//         // run thread task in parallel
//         if (task.checkNum)  // if the thread got a number from `nums`
//         {
//             if (thread_is_prime(*task.num_to_check))  // check is the `num_to_check` prime
//             {
//                 // add the prime to `result`
//                 myMutex.lock();
//                 cout << "Thread #" << thread_id << " found that number " << *task.num_to_check << " IS prime." << endl;    // debug
//                 result.emplace_back(*task.num_to_check);
//                 myMutex.unlock();
//             }
//             // debug
//             else
//             {
//                 myMutex.lock();
//                 cout << "Thread #" << thread_id << " found that number " << *task.num_to_check << " is NOT prime." << endl;    // debug
//                 myMutex.unlock();
//             }
//             task.checkNum = false;
//         }

//         // check if all the numbers in `nums` have been checked
//         if (finished)
//         {
//             myMutex.lock();
//             cout << "[FINISHED] Thread #" << thread_id << " exiting..." << endl;    // debug
//             myMutex.unlock();
//             break;
//         }
//     }
// }
// */

// // This function takes a list of numbers nums[] and returns a subset of them
// // that are primes. Duplicates in input must be duplicated in the output.
// //
// // The parameter n_threads indicates how many threads should be created to speed
// // up the computation.
// // -----------------------------------------------------------------------------
// // You will most likely need to re-implement this function entirely.
// // Note that the current implementation ignores n_threads. Your multithreaded
// // implementation must use this parameter.
// // std::vector<int64_t> detect_primes(const std::vector<int64_t> & nums, int n_threads)
// // std::vector<int64_t> detect_primes(const std::vector<int64_t> & nums, int n_threads)
// // {
// //     // create threads
// //     Task task_pool[n_threads];
// //     thread thread_pool[n_threads];
// //     for (int i = 0; i < n_threads; i++)
// //     {
// //         // task_pool[i] = {};
// //         thread_pool[i] = thread(thread_function, nums, i, task_pool[i]);
// //     }

// //     // check if `nums` is empty
// //     if (nums.empty())
// //         finished = true;

// //     // start up threads
// //     for (int i = 0; !finished && i < n_threads; i++)
// //     {
// //         if (thread_pool[i].joinable())
// //             thread_pool[i].join();
// //     }

// //     // needed ?
// //     // // keep this thread looped
// //     // while (!finished)
// //     // {}

// //     return result;
// // }

// std::vector<int64_t> detect_primes(const std::vector<int64_t> & nums, int n_threads)
// {
//     // check if `nums` is empty
//     if (nums.empty())
//         finished = true;

//     // create threads
//     Task task_pool[n_threads];
//     thread thread_pool[n_threads];
//     for (int i = 0; i < n_threads; i++)
//     {
//         // task_pool[i] = {};
//         thread_pool[i] = thread(thread_function, nums, i, task_pool[i]);
//     }

//     return result;
// }



