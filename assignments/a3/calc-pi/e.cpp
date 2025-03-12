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

#include <cmath>
#include "calcpi.h"
#include <thread>
#include <vector>
#include <stdio.h>
#include <inttypes.h>


struct alignas(64) Task {double start_x, end_x; uint64_t partial_count;};

void approx_thread(Task& thread, int r){
    double rsq = double(r) * r;
    for (double x = thread.start_x; x <= thread.end_x; x++) {
        double xSquared = x * x;
        for (double y = -r; y <= r; y++) {
            if (xSquared + y * y <= rsq) {
                thread.partial_count++;
            }
        }
    }

}
/*
void approx_thread(Task& task, int r) {
    double rsq = double(r) * r;
    for (double x = task.start_x; x <= task.end_x; x++) {
        double xSquared = x * x;
        double maxY = std::sqrt(rsq - xSquared);
        for (double y = -maxY; y <= maxY; y++) {
            task.partial_count++;
        }
    }
}
*/

uint64_t count_pixels(int r, int n_threads)
{

    Task tasks[256];
    std::thread threads[256];

    // initialize
    // each thread starts at current start value 
    // since threads can be uneven, have something to add remainder to all threads until impossible

    int remaining = (r * 2) + 1;
    int offset = remaining / n_threads;
    int extra = remaining % n_threads;
    int start = -r;

    for (int i = 0; i < n_threads; i++){
        int toAdd = offset + (i < extra ? 1 : 0);

        tasks[i].start_x = start;
        tasks[i].end_x = start + toAdd - 1;
        tasks[i].partial_count = 0;

        start += toAdd;
        threads[i] = std::thread(approx_thread, std::ref(tasks[i]), r);
    }

    uint64_t total_pixels = 0;
    for (int i = 0; i < n_threads; i++){
        if (threads[i].joinable()) threads[i].join();
        total_pixels += tasks[i].partial_count;
    }

    //printf("%" PRIu64 "\n", total_pixels);

    // make sure your multithreaded solution returns the same number of
    // pixels as the single-threaded solution below!!!
    // kept for testing purposes
    /* double rsq = double(r) * r;
    uint64_t count = 0;
    for (double x = -r; x <= r; x++) {
        for (double y = -r; y <= r; y++) {
            if (x * x + y * y <= rsq) {
                count++;
            }
        }
    }
    */
    return total_pixels;
}
