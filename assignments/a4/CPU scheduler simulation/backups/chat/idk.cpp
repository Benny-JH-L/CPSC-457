/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the only file you should modify and submit for grading

#include "scheduler.h"
#include <queue>
#include <algorithm>

void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) 
{
    // Clear the sequence
    seq.clear();

    // Sort processes by arrival time
    std::sort(processes.begin(), processes.end(), 
        [](const Process& a, const Process& b) { return a.arrival < b.arrival; });

    // Create a copy of processes to track remaining burst times
    std::vector<Process> remaining_procs = processes;
    for (auto& p : remaining_procs) {
        p.start_time = -1;
        p.finish_time = -1;
    }

    // Ready queue for processes
    std::queue<int> ready_queue;

    // Current time tracker
    int64_t current_time = 0;

    // Find the first arriving process
    bool found_first = false;
    for (size_t i = 0; i < remaining_procs.size(); ++i) {
        if (remaining_procs[i].arrival <= current_time) {
            ready_queue.push(i);
            found_first = true;
        }
    }

    // If no processes are ready, advance time
    if (!found_first) {
        current_time = processes[0].arrival;
        ready_queue.push(0);
    }

    // Track whether the CPU was idle
    bool was_idle = (ready_queue.empty());

    // Compressed execution sequence
    while (!ready_queue.empty()) {
        // Get the current process index
        int current_proc_idx = ready_queue.front();
        ready_queue.pop();

        Process& current_proc = remaining_procs[current_proc_idx];

        // Set start time if not already set
        if (current_proc.start_time == -1) {
            current_proc.start_time = current_time;
        }

        // Determine execution time (minimum of quantum and remaining burst)
        int64_t exec_time = std::min(quantum, current_proc.burst);

        // Update sequence (compressed)
        if (seq.empty() || seq.back() != current_proc.id) {
            if (seq.size() < max_seq_len) {
                seq.push_back(current_proc.id);
            }
        }

        // Update current time
        current_time += exec_time;
        current_proc.burst -= exec_time;

        // Check for newly arriving processes
        for (size_t i = 0; i < remaining_procs.size(); ++i) {
            if (remaining_procs[i].arrival <= current_time && 
                remaining_procs[i].start_time == -1) {
                ready_queue.push(i);
            }
        }

        // Process not finished, put back in queue
        if (current_proc.burst > 0) {
            ready_queue.push(current_proc_idx);
        } 
        // Process finished
        else {
            current_proc.finish_time = current_time;
        }

        // Check for idle time
        if (ready_queue.empty()) {
            // If previously not idle, add -1 to sequence (idle)
            if (!was_idle && seq.size() < max_seq_len) {
                seq.push_back(-1);
            }
            was_idle = true;

            // Find next arriving process
            int64_t next_arrival = INT64_MAX;
            int next_proc_idx = -1;
            for (size_t i = 0; i < remaining_procs.size(); ++i) {
                if (remaining_procs[i].finish_time == -1 && 
                    remaining_procs[i].arrival > current_time &&
                    remaining_procs[i].arrival < next_arrival) {
                    next_arrival = remaining_procs[i].arrival;
                    next_proc_idx = i;
                }
            }

            // Advance time and add process if found
            if (next_proc_idx != -1) {
                current_time = next_arrival;
                ready_queue.push(next_proc_idx);
                was_idle = false;
            }
        } else {
            was_idle = false;
        }

        // Break if max sequence length reached
        if (seq.size() >= max_seq_len) break;
    }

    // Copy start and finish times back to original processes
    for (size_t i = 0; i < processes.size(); ++i) {
        processes[i].start_time = remaining_procs[i].start_time;
        processes[i].finish_time = remaining_procs[i].finish_time;
    }
}
