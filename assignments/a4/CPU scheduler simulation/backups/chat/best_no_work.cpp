#include "scheduler.h"
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) 
{
    seq.clear();
    if (processes.empty()) return;
    
    int64_t current_time = 0;
    vector<int64_t> remaining_time(processes.size());
    for (size_t i = 0; i < processes.size(); i++)
        remaining_time[i] = processes[i].burst;
    
    queue<int> job_queue, ready_queue;
    for (size_t i = 0; i < processes.size(); i++)
        job_queue.push(processes[i].id);
    
    vector<int> sequence;
    while (true)
    {
        if (ready_queue.empty())
        {
            if (job_queue.empty()) break;
            while (!job_queue.empty() && processes[job_queue.front()].arrival <= current_time)
            {
                ready_queue.push(job_queue.front());
                job_queue.pop();
            }
            if (ready_queue.empty())
            {
                ready_queue.push(job_queue.front());
                job_queue.pop();
            }
            int id = ready_queue.front();
            current_time = max(current_time, processes[id].arrival);
            sequence.emplace_back(-1);
        }
        
        int rq_size = ready_queue.size();
        int64_t min_rem_time = INT64_MAX;
        for (int i = 0; i < rq_size; i++)
        {
            int proc_id = ready_queue.front();
            ready_queue.pop();
            min_rem_time = min(min_rem_time, remaining_time[proc_id]);
            ready_queue.push(proc_id);
        }
        
        int64_t max_jump = min_rem_time / quantum;
        if (max_jump > 0 && sequence.size() >= max_seq_len)
        {
            current_time += max_jump * rq_size * quantum;
            for (int i = 0; i < rq_size; i++)
            {
                int proc_id = ready_queue.front();
                ready_queue.pop();
                remaining_time[proc_id] -= max_jump * quantum;
                ready_queue.push(proc_id);
            }
            continue;
        }
        
        int proc_rq = ready_queue.front();
        ready_queue.pop();
        sequence.emplace_back(proc_rq);
        
        if (processes[proc_rq].start_time == -1)
            processes[proc_rq].start_time = current_time;
        
        int64_t execute_time = min(quantum, remaining_time[proc_rq]);
        current_time += execute_time;
        remaining_time[proc_rq] -= execute_time;
        
        if (remaining_time[proc_rq] <= 0)
            processes[proc_rq].finish_time = current_time;
        
        while (!job_queue.empty() && processes[job_queue.front()].arrival <= current_time)
        {
            ready_queue.push(job_queue.front());
            job_queue.pop();
        }
        
        if (remaining_time[proc_rq] > 0)
            ready_queue.push(proc_rq);
    }
    
    if (max_seq_len > 0 && !sequence.empty())
    {
        seq.emplace_back(sequence[0]);
        for (size_t i = 1; i < sequence.size() && seq.size() < max_seq_len; i++)
            if (sequence[i] != seq.back())
                seq.emplace_back(sequence[i]);
    }
}

does not implment best solution properly