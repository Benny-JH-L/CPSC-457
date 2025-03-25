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
    // Clear the output sequence 
    seq.clear();

    // Sort processes by arrival time
    std::sort(processes.begin(), processes.end(), 
        [](const Process& a, const Process& b) {
            return a.arrival < b.arrival;
        });

    // Track remaining burst time for each process
    std::vector<int64_t> remaining_burst(processes.size());
    for (size_t i = 0; i < processes.size(); ++i) {
        remaining_burst[i] = processes[i].burst;
    }

    // Ready queue for processes
    std::queue<int> ready_queue;

    // Variables to track simulation state
    int64_t current_time = 0;
    bool cpu_idle = true;
    int current_process = -1;
    int next_process_index = 0;

    // Run the simulation
    while (true) {
        // Check for new arriving processes
        while (next_process_index < processes.size() && 
               processes[next_process_index].arrival <= current_time) {
            ready_queue.push(next_process_index);
            next_process_index++;
        }

        // Check if no processes are left to schedule
        if (ready_queue.empty() && next_process_index >= processes.size()) {
            break;
        }

        // If CPU is idle and there are processes in the queue
        if (cpu_idle && !ready_queue.empty()) {
            current_process = ready_queue.front();
            ready_queue.pop();
            
            // Set start time if not already set
            if (processes[current_process].start_time == -1) {
                processes[current_process].start_time = current_time;
            }
            
            cpu_idle = false;
        }

        // If CPU is idle, add -1 to sequence
        if (cpu_idle) {
            // Compress sequence (no repeated consecutive -1)
            if (seq.empty() || seq.back() != -1) {
                seq.push_back(-1);
            }
            current_time++;
            continue;
        }

        // Execute process for minimum of quantum or remaining burst
        int64_t execute_time = std::min(quantum, remaining_burst[current_process]);
        
        // Compress sequence (no repeated consecutive process id)
        if (seq.empty() || seq.back() != processes[current_process].id) {
            seq.push_back(processes[current_process].id);
        }

        // Update remaining burst and current time
        remaining_burst[current_process] -= execute_time;
        current_time += execute_time;

        // Check for new arriving processes during execution
        while (next_process_index < processes.size() && 
               processes[next_process_index].arrival <= current_time) {
            ready_queue.push(next_process_index);
            next_process_index++;
        }

        // Process completed
        if (remaining_burst[current_process] == 0) {
            processes[current_process].finish_time = current_time;
            cpu_idle = true;
        }
        // Time slice exhausted, move process to end of queue
        else if (execute_time == quantum) {
            ready_queue.push(current_process);
            cpu_idle = true;
        }

        // Truncate sequence if it exceeds max_seq_len
        if (seq.size() >= max_seq_len) {
            seq.resize(max_seq_len);
            break;
        }
    }
}
