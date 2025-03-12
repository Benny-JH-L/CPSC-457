// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================
//
// count_pixels() counts the number of pixels that fall into a circle
// using the algorithm explained here:
//
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pixels() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//
// Currently the function ignores the n_threads parameter. Your job is to
// parallelize the function so that it uses n_threads threads to do
// the computation.


/*
CPSC457 Winter 2025 | Assignment 3 | Benny Liang | 30192142
*/

#include "calcpi.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <thread>

using namespace std;

struct Task 
{
    double start_x;
    double end_x;
    uint64_t partial_count = 0;
    int radius = 0;
};

void count_pixels_in_range(Task* task) 
{
    int r = task->radius;
    double r_squared = double(r) * r;
    uint64_t local_partial_count = 0;
    for (double x = task->start_x; x < task->end_x; ++x) 
    {
        for (double y = -r; y <= r; ++y) 
        {
            if (x * x + y * y <= r_squared) 
                local_partial_count++;
        }
    }
    task->partial_count = local_partial_count;
}

uint64_t count_pixels(int r, int n_threads)
{

    vector<thread> threads;
    vector<Task> tasks(n_threads);

    int total_columns = (2 * r) + 1;
    int columns_per_thread = total_columns / n_threads;
    int remaining_cols = total_columns % n_threads;
    int current_start = -r;

    // create threads and allocate values to their task
    for(int i = 0; i < n_threads; i++) 
    {
        tasks[i].start_x = current_start;
        
        // distribute remaining columns evenly
        int thread_columns = columns_per_thread;
        if (i < remaining_cols) 
            thread_columns++;
        
        tasks[i].end_x = current_start + thread_columns;
        current_start = tasks[i].end_x;
        tasks[i].radius = r;
        threads.emplace_back(count_pixels_in_range, &tasks[i]);
    }

    uint64_t count = 0;
    for (int i = 0; i < n_threads; i++) 
    {
        if (threads[i].joinable()) 
            threads[i].join();
        count += tasks[i].partial_count;
    }

    // make sure your multithreaded solution returns the same number of
    // pixels as the single-threaded solution below!!!
    // double rsq = double(r) * r;
    // uint64_t count = 0;
    // for (double x = -r; x <= r; x++) 
    // {
    //     printf("\n--x = %f\n", x);
    //     for (double y = -r; y <= r; y++) 
    //     {
    //         if (x * x + y * y <= rsq) 
    //         {
    //             count++;
    //             printf("count = %ld\n", count);
    //     }
    //         printf("y = %f\n", y);
    //     }
    // }

    // cout << "Final count: " << count << endl;   // debug
    return count;
}

// need to make n threads, then make them count_pixels with evenly distributed radi
// then, sum up all the partials counts at the end
 
// uint64_t count_pixels(int r, int n_threads)
// {
//     // make sure your multithreaded solution returns the same number of
//     // pixels as the single-threaded solution below!!!
//     double rsq = double(r) * r;
//     uint64_t count = 0;
//     for (double x = -r; x <= r; x++) {
//         for (double y = -r; y <= r; y++) {
//             if (x * x + y * y <= rsq) {
//                 count++;
//             }
//         }
//     }
//     return count;
// }
