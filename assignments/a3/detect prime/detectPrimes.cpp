/// ============================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// ============================================================================
///
/// You must modify this file and then submit it for grading to D2L.
///
/// You can delete all contents of this file and start from scratch if
/// you wish, as long as you implement the detect_primes() function as
/// defined in "detectPrimes.h".


/*
CPSC457 Winter 2025 | Assignment 3 | Benny Liang | 30192142
*/

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

struct Task
{
    int64_t curr_num = 0;
    int64_t start = 0;
    int64_t end = 0;
};


// C++ barrier class (from lecture notes).
// You do not have to use it in your implementation. If you don't use it, you
// may delete it.
class simple_barrier {
    std::mutex m_;
    std::condition_variable cv_;
    int n_remaining_, count_;
    bool coin_;
  
    public: 
    simple_barrier(int count = 1) { init(count); }
    void init(int count)
    {
      count_ = count;
      n_remaining_ = count_;
      coin_ = false;
    }
    bool wait()
    {
      if (count_ == 1) return true;
      std::unique_lock<std::mutex> lk(m_);
      if (n_remaining_ == 1) {
        coin_ = ! coin_;
        n_remaining_ = count_;
        cv_.notify_all();
        return true;
      }
      auto old_coin = coin_;
      n_remaining_--;
      cv_.wait(lk, [&]() { return old_coin != coin_; });
      return false;
    }
  };

unordered_map<int64_t, bool> cached_results; // a cache, remembers previously computed numbers if they were prime.

mutex myMutex;  // used for debug statments

atomic<bool> global_is_number_selected = false;
atomic<bool> global_finished = false;               // flag for all threads, if we've gone through all numbers in the `nums` vector

int global_n_threads;                               // number of threads

vector<int64_t> global_result;                      // stores numbers that are prime
vector<Task> global_tasks;                          // values for each thread (thread_id -> associated task)

/// Task -> The thread's associated task information
/// is_prime -> flag to tell this thread and other threads if the current number is a prime.
/// is another thread determines that this number is prime, all other threads will exit early.
void check_prime_task(Task& task, atomic<bool>& is_prime) 
{
    int64_t number = task.curr_num;
    int64_t start = task.start;
    int64_t end = task.end;

    // debug
    // cout << "number to check: " << number << endl;
    // cout << "start: " << start << endl;
    // cout << "end: " << end << endl;

    for (int64_t i = start; i <= end && is_prime.load(); i += 6)
    {
        if (number % i == 0 || number % (i + 2) == 0) 
        {
            is_prime.store(false);
            return;
        }
    }
}

void thread_function(simple_barrier& barrier, int thread_id, const vector<int64_t>& nums, unsigned long& curr_index, atomic<bool>& is_prime)
{
    while(true)
    {

        if (barrier.wait())     // get the `last` thread to reach the barrier
        {
            // debug
            // myMutex.lock();
            // cout << "Chosen: thread #" << thread_id << endl;    
            // cout << "\tCurr index = " << curr_index;
            // myMutex.unlock();

            if (curr_index >= nums.size())  // check if we've gone through all numbers in the vector
            {
                // cout << "\nExiting..." << thread_id << endl;    // debug
                global_finished = true;
            }
            else
            {
                // cout << "| Val: " << nums[curr_index] << endl; // debug

                int64_t number = nums[curr_index];
                if (cached_results.count(number))               // check cache if `number` was checked primality previously
                {                                               // Note: checking cache add's to run time, so this is effective when there are many of the same number who are very very large. 
                    if (cached_results[number] == true)         // `number` is a prime
                    {
                        global_result.emplace_back(number);
                        global_is_number_selected.store(false);
                    }
                }
                // Trivial checks if `number` is a prime or not
                else if (number < 2)
                    global_is_number_selected.store(false);
                else if (number <= 3) 
                {
                    global_result.emplace_back(number);
                    global_is_number_selected.store(false);
                }
                else if (number % 2 == 0 || number % 3 == 0)
                    global_is_number_selected.store(false);
                // end of trivial checks
                else
                {
                    int64_t limit = sqrt(number);
                    int64_t step = (limit - 5) / 6 + 1;
                    int64_t chunk_size = step / global_n_threads;
                    int remainder = step % global_n_threads;

                    // debug
                    // cout << "limit is: " << limit << endl;
                    // cout << "step is: " << step << endl;
                    // cout << "chunk_size is: " << chunk_size << endl;
                    // cout << "remainder is: " << remainder << endl;

                    // Allocate task values for threads (the range of divisors to check for each thread)
                    int64_t current_index = 0;
                    // some of the code in the for-loop was from tutorial hints and then altered
                    for (int i = 0; i < global_n_threads; i++)
                    {
                        int64_t start = 5 + (current_index * 6);        // beginning of the range
                        int64_t end = start + (chunk_size * 6) - 1;     // end of the range

                        if (i < remainder)  // allocate remainders
                            end += 6;
                        if (end > limit)    // cap the end to the `limit`
                            end = limit;
                        
                        // Set task values for thread `i`
                        global_tasks[i].curr_num = number;
                        global_tasks[i].start = start;
                        global_tasks[i].end = end;

                        // debug
                        // cout << "task["<<i<< "].currN = " << global_tasks[i].curr_num << endl;
                        // cout << "task["<<i<< "].start = " << global_tasks[i].start << endl;
                        // cout << "task["<<i<< "].end = " << global_tasks[i].end << endl;

                        current_index += chunk_size + (i < remainder);  // add the `chunk_size`, and 1 if (i < remainder), adds otherwise 0.
                    }
                    global_is_number_selected.store(true);
                    // cout << "fin allocating to threads" << endl;    // debug
                }
                curr_index++;   // check next number in the vector
            }
        }

        // synchronize again before starting parallel work
        barrier.wait();

        // if gone through all the numbers in `nums` then threads can break
        if (global_finished)
            break;
        
        // parallel task
        if (global_is_number_selected)
        {
            // debug
            // myMutex.lock();
            // cout << "Thread #" << thread_id << " entering task..." << endl;    
            // myMutex.unlock();

            check_prime_task(global_tasks[thread_id], ref(is_prime));   // each thread will check its task range

            // wait until all threads are done.
            if (barrier.wait())         // get last thread
            {
                // cout << "prime check" << endl;  /// debug
                if (is_prime)
                {
                    global_result.emplace_back(global_tasks[thread_id].curr_num);
                }
                cached_results[global_tasks[thread_id].curr_num] = is_prime;
                global_is_number_selected.store(false);
                is_prime.store(true);
            }
        }

        // wait for thread who entered above `if` block to finish
        barrier.wait();
    }
}

// This function takes a list of numbers nums[] and returns a subset of them
// that are primes. Duplicates in input must be duplicated in the output.
//
// The parameter n_threads indicates how many threads should be created to speed
// up the computation.
std::vector<int64_t> detect_primes(const std::vector<int64_t> & nums, int n_threads)
{
    // check if `nums` is empty
    if (nums.empty())
        return {};

    // create threads and tasks
    vector<Task> task_pool(n_threads, {0, 0, 0});    // create `n_thread` instances of global_Tasks with {0, 0, 0} as values in struct 
    vector<thread> thread_pool;
    unsigned long currentIndex = 0;

    atomic<bool> is_prime = true;
    simple_barrier barrier = simple_barrier(n_threads);

    for (int i = 0; i < n_threads; i++)
    {
        thread_pool.emplace_back(thread(thread_function, ref(barrier), i, ref(nums), ref(currentIndex), ref(is_prime)));
    }
    // void thread_function(int thread_id, const vector<int64_t>& nums, vector<Task>& global_tasks, Task& this_task, unsigned long& curr_index, atomic<bool>& is_prime)

    global_tasks = task_pool;
    global_n_threads = n_threads;

    // start up threads
    for (int i = 0; i < n_threads; i++)
    {
        if (thread_pool[i].joinable())
            thread_pool[i].join();
    }

    // cout << "Done!" << endl;    // debug

    return global_result;
}
