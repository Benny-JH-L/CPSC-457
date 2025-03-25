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

    // If no processes, return
    if (processes.empty()) return;

    // Sort processes by arrival time (they should already be sorted, but just in case)
    std::sort(processes.begin(), processes.end(), 
        [](const Process& a, const Process& b) { return a.arrival < b.arrival; });

    // Initialize variables
    std::queue<Process*> ready_queue;
    int64_t current_time = 0;
    size_t next_process_index = 0;

    // Track remaining burst for each process
    std::vector<int64_t> remaining_burst(processes.size());
    for (size_t i = 0; i < processes.size(); ++i) {
        remaining_burst[i] = processes[i].burst;
    }

    // Continue until all processes are done and no more are arriving
    while (next_process_index < processes.size() || !ready_queue.empty()) {
        // Add newly arrived processes to the ready queue
        while (next_process_index < processes.size() && 
               processes[next_process_index].arrival <= current_time) {
            ready_queue.push(&processes[next_process_index]);
            next_process_index++;
        }

        // If no process is ready, move time to next process arrival
        if (ready_queue.empty()) {
            // If idle, add -1 to sequence
            if (seq.empty() || seq.back() != -1) {
                seq.push_back(-1);
            }
            
            // Fast forward time to next process arrival
            current_time = processes[next_process_index].arrival;
            continue;
        }

        // Get next process from ready queue
        Process* current_process = ready_queue.front();
        ready_queue.pop();

        // Set start time if not already set
        if (current_process->start_time == -1) {
            current_process->start_time = current_time;
        }

        // Add process ID to sequence
        if (seq.empty() || seq.back() != current_process->id) {
            seq.push_back(current_process->id);
        }

        // Determine execution time (minimum of quantum and remaining burst)
        int64_t execution_time = std::min(quantum, remaining_burst[current_process->id]);
        current_time += execution_time;
        remaining_burst[current_process->id] -= execution_time;

        // Add newly arrived processes before putting current process back in queue
        while (next_process_index < processes.size() && 
               processes[next_process_index].arrival <= current_time) {
            ready_queue.push(&processes[next_process_index]);
            next_process_index++;
        }

        // If process is not done, put it back in the queue
        if (remaining_burst[current_process->id] > 0) {
            ready_queue.push(current_process);
        }
        else {
            // Process is complete
            current_process->finish_time = current_time;
        }
    }

    // Trim sequence to max_seq_len
    if (seq.size() > max_seq_len) {
        seq.resize(max_seq_len);
    }
}
