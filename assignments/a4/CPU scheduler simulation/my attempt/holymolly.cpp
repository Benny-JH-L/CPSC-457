#include "scheduler.h"
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

using namespace std;

void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) 
{
    seq.clear();

    if (processes.empty())
        return;

    int64_t current_time = 0;
    vector<int64_t> vec_remaining_proc_time(processes.size());
    
    // Set process burst times
    for (uint64_t i = 0; i < vec_remaining_proc_time.size(); i++)
        vec_remaining_proc_time[i] = processes[i].burst;

    vector<int> job_queue, ready_queue;
    int64_t starts_recorded = 0;
    int64_t starts_max = processes.size();
    
    // add all the process id's to job queue
    for (uint64_t i = 0; i < processes.size(); i++)
        job_queue.push_back(processes[i].id);

    vector<int> sequence;
    while (true)
    {
        // finish if done
        if (job_queue.empty() && ready_queue.empty())
            break;

        if (!job_queue.empty() && ready_queue.empty())
        {
            // get next process
            int64_t next_process = job_queue[0];

            // jump to arrival of next process
            current_time = processes[next_process].arrival;
            
            // set start time if not set
            if (processes[next_process].start_time == -1)
            {
                processes[next_process].start_time = current_time;
                starts_recorded++;
            }

            // if starting after 0, push in -1 to indicate idle state
            if (current_time > 0 && seq.size() < max_seq_len)
                seq.push_back(-1);

            // remove from job queue, add to ready queue
            job_queue.erase(job_queue.begin());
            ready_queue.push_back(next_process);
        }

        // if job queue is empty but ready queue isn't
        if (job_queue.empty() && !ready_queue.empty())
        {
            // get next process
            int curr_process = ready_queue[0];
            ready_queue.erase(ready_queue.begin());

            // record start if haven't
            if (processes[curr_process].start_time == -1)
            {
                processes[curr_process].start_time = current_time;
                starts_recorded++;
            }

            // execute process
            int64_t execute_time = min(quantum, vec_remaining_proc_time[curr_process]);
            current_time += execute_time;
            vec_remaining_proc_time[curr_process] -= execute_time;

            // if finished, set end time
            if (vec_remaining_proc_time[curr_process] == 0)
                processes[curr_process].finish_time = current_time;
            // else push back to ready queue
            else
                ready_queue.push_back(curr_process);

            // push to seq
            if (seq.empty() || ((seq.back() != curr_process) && seq.size() < max_seq_len))
                seq.push_back(curr_process);
        }

        if (!job_queue.empty() && !ready_queue.empty())
        {
            // get next process
            int curr_process = ready_queue[0];
            ready_queue.erase(ready_queue.begin());

            // record start if haven't
            if (processes[curr_process].start_time == -1)
            {
                processes[curr_process].start_time = current_time;
                starts_recorded++;
            }

            vector<int64_t> same_time;

            // execute process
            int64_t execute_time = min(quantum, vec_remaining_proc_time[curr_process]);
            current_time += execute_time;
            vec_remaining_proc_time[curr_process] -= execute_time;

            // if finished, set finish time
            if (vec_remaining_proc_time[curr_process] == 0)
                processes[curr_process].finish_time = current_time;

            // check for processes arriving during the quantum
            for (auto id = job_queue.begin(); id != job_queue.end();) 
            {
                if (processes[*id].arrival < current_time) 
                {
                    ready_queue.push_back(*id);
                    id = job_queue.erase(id);
                } 
                else if (processes[*id].arrival == current_time)
                {
                    same_time.push_back(*id);
                    id = job_queue.erase(id);
                }
                else 
                {
                    ++id;
                }
            }

            // push back if not done
            if (vec_remaining_proc_time[curr_process] > 0)
                ready_queue.push_back(curr_process);

            // push back everything that arrived at same time
            for (int id : same_time)
                ready_queue.push_back(id);

            // push to seq if a new process
            if (seq.empty() || (seq.back() != curr_process) && seq.size() < max_seq_len)
                seq.push_back(curr_process);
        }

        // optimization for large quantum and burst times
        if (!ready_queue.empty())
        {
            if (((seq.size() == max_seq_len) || (ready_queue.size() == 1 && seq.size() < max_seq_len)) && starts_recorded >= ready_queue.size())
            {
                // find minimum remaining burst
                int64_t min_remaining_burst = INT64_MAX;
                int64_t rounds = ready_queue.size();

                for (int id : ready_queue)
                    min_remaining_burst = min(vec_remaining_proc_time[id], min_remaining_burst);

                int64_t n1_val = INT64_MAX;
                int64_t n2_val = INT64_MAX;

                // calculate maximum iterations for minimum remaining burst
                if (quantum < min_remaining_burst)
                    n1_val = (min_remaining_burst / quantum) - 1;

                // calculate jump to next arrival
                if (!job_queue.empty())
                    n2_val = (processes[job_queue[0]].arrival - current_time) / (rounds * quantum);

                int64_t n_val = min(n1_val, n2_val);

                // apply optimization if valid
                if (n_val != INT64_MAX)
                {
                    current_time += n_val * rounds * quantum;

                    for (int id : ready_queue)
                        vec_remaining_proc_time[id] -= n_val * quantum;
                }
            }
        }
    }
}