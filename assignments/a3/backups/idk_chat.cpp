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
#include <vector>

using namespace std;

// Returns true if n is prime
bool is_prime(int64_t n) {
    if (n < 2) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    int64_t i = 5, max = sqrt(n);
    while (i <= max) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
        i += 6;
    }
    return true;
}

// Shared variables
mutex resultMutex;
unordered_map<int64_t, bool> primeCache;
vector<int64_t> result;
atomic<size_t> curr_index(0);

// Thread function
void thread_function(const vector<int64_t>& nums) {
    while (true) {
        size_t index = curr_index.fetch_add(1);
        if (index >= nums.size()) break;  // No more numbers to check

        int64_t num = nums[index];

        // Check if number is already computed
        bool isPrime;
        {
            lock_guard<mutex> lock(resultMutex);
            if (primeCache.count(num)) {
                isPrime = primeCache[num];
                continue;  // Skip computation
            }
        }

        // Compute primality
        isPrime = is_prime(num);

        // Store result
        lock_guard<mutex> lock(resultMutex);
        primeCache[num] = isPrime;
        if (isPrime) result.push_back(num);
    }
}

// Main function
vector<int64_t> detect_primes(const vector<int64_t>& nums, int n_threads) {
    if (nums.empty()) return {};  // Edge case: empty input

    curr_index = 0;  // Reset atomic index
    result.clear();
    primeCache.clear();

    // Create and start threads
    vector<thread> threads;
    for (int i = 0; i < n_threads; i++) {
        threads.emplace_back(thread_function, ref(nums));
    }

    // Join threads
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    return result;
}
