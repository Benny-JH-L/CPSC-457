/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the only file you should modify and submit for grading

#include "scheduler.h"
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

// debugging
void print_queue(queue<int> copy_q)
{
    while (!copy_q.empty())
    {
        cout << copy_q.front() << ", ";
        copy_q.pop();
    }
    cout << endl;
}

// this is the function you should implement
//
// simulate_rr() implements a Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the compressed execution sequence
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//         - sequence will be trimmed to contain only first max_seq_len entries
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//
void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) 
{
    // for (auto& p : processes)
    // {
    //     cout << "proc<"<<p.id<<">\n \tarrival: " << p.arrival << "\n\tburst: " << p.burst << endl; 
    // }
    seq.clear();

    int64_t current_time = 0;
    vector<int64_t> vec_remaining_proc_time(processes.size(), 0);    // index == process ID, and it contains the remaining time it has until it finishes

    // Set process burst times
    for (uint64_t i = 0; i < vec_remaining_proc_time.size(); i++)
        // vec_remaining_proc_time[processes[i].id] = processes[i].burst;  // since processes is sorted by arrival time and ID this should be equivalent: vec_remaining_proc_time[i] = processes[i].burst
        vec_remaining_proc_time[i] = processes[i].burst;

    // ID's are `ints`
    queue<int> job_queue;
    queue<int> ready_queue;
    
    // add all the process id's to job queue
    for (uint64_t i = 0; i < processes.size(); i++)
        job_queue.push(processes[i].id);

    cout << "\ninitial job queue: " << endl;
    print_queue(job_queue);

    vector<int> sequence;
    while (true)
    {
        cout << "\ncurrent time: " << current_time << endl;

        if (ready_queue.size() == 0 || ready_queue.empty())     // need to add proccesses to ready queue (rq)
        {
            cout << "ready queue is empty\n";   // debug
    
            if (job_queue.size() == 0)                          // no more processes to execute (ie no more to add to rq)
            {
                cout << "job queue is empty (exiting...)\n";    // debug
                break;
            }

            // add processes to ready queue from the job queue, whose arrival time <= current time
            while (!job_queue.empty()) 
            {
                int proc_jq = job_queue.front();
                cout << "proc id <"<<proc_jq<<"> arrival: " << processes[proc_jq].arrival << endl;  // debug
                if (processes[proc_jq].arrival <= current_time)
                {
                    ready_queue.push(proc_jq);
                    job_queue.pop();
                    cout << "\tadded proc id <"<<proc_jq<<"> to RQ\n";  // debug
                }
                // break early since this proccesses arrival time > current time
                else
                {
                    cout << "\t1left RQ adder early\n"; // debug
                    break;
                }
            }

            // if the ready queue is still empty, then the next proc has an arrival time > curr time,
            // add it to the ready queue.
            if (ready_queue.empty())
            {
                ready_queue.push(job_queue.front());
                job_queue.pop();
            }

            int id = ready_queue.front();
            
            if (current_time < processes[id].arrival)   // idle
            {
                sequence.emplace_back(-1);
                cout << "idling...";
            }
            // debug
            cout << "using id<"<<id<<"> arrival to update curr time (if it is > curr time)\n";
            cout << "\t arrival: " << processes[id].arrival << endl;

            current_time = max(current_time, processes[id].arrival);

            cout << "current time: " << current_time << endl;   // debug

            // debugging
            cout << "\nready queue: " << endl;
            print_queue(ready_queue);
            cout << "\njob queue: " << endl;
            print_queue(job_queue);
        }

        int proc_rq = ready_queue.front();
        ready_queue.pop();
        sequence.emplace_back(proc_rq);

        if (processes[proc_rq].start_time == -1)
        {
            processes[proc_rq].start_time = current_time;
            cout << "proc id<"<<proc_rq<<"> starting exec. at: " << current_time << endl; // debug
        }
        cout << "executing proc id<"<<proc_rq<<"> with rem. time: " << vec_remaining_proc_time[proc_rq] << endl;
        cout << "curr_time before execution: " << current_time << endl;
        // int64_t proc_remaining_time = vec_remaining_proc_time[proc_rq];
        // int64_t execute_time = min(quantum, proc_remaining_time);
        int64_t execute_time = min(quantum, vec_remaining_proc_time[proc_rq]);
        current_time += execute_time;
        vec_remaining_proc_time[proc_rq] -= execute_time;
        // proc_remaining_time -= execute_time;
        // vec_remaining_proc_time[proc_rq] = proc_remaining_time;

        cout << "curr_time after execution: " << current_time << endl;
        
        if (vec_remaining_proc_time[proc_rq] <= 0)
        {
            processes[proc_rq].finish_time = current_time;
            cout << "(curr proc) proc id<"<<proc_rq<<"> finished at time: " << current_time << endl;
        }

        while (!job_queue.empty())
        {
            int proc_jq = job_queue.front();
            cout << "proc id <"<<proc_jq<<"> arrival: " << processes[proc_jq].arrival << endl;  // debug
            if (processes[proc_jq].arrival < current_time)
            {
                ready_queue.push(proc_jq);
                job_queue.pop();
                cout << "\tadded proc id <"<<proc_jq<<"> to RQ\n";  // debug
            }
            // break early since this proccesses arrival time > current time
            else
            {
                cout << "\t2left RQ adder early\n"; // debug
                break;  // break early
            }
        }

        // check if the current proccess is not finished, ie burst time > 0
        if (vec_remaining_proc_time[proc_rq] > 0)
        {
            ready_queue.push(proc_rq);  // add the current proccess back to the ready queue
            cout << "(curr proc) proc id<"<<proc_rq<<"> has rem. time: "<<vec_remaining_proc_time[proc_rq]<<" adding back to RQ\n"; // debug
        }

        // debugging
        cout << "\nready queue: " << endl;
        print_queue(ready_queue);
        cout << "\njob queue: " << endl;
        print_queue(job_queue);
    }

    cout << "sequence (uncompresssed):\n[";
    for (auto& id : sequence)
    {
        cout << id <<",";
    }
    cout << endl;

    // compress the sequence
    if (max_seq_len > 0 && sequence.size() > 0)
    {
        seq.emplace_back(sequence[0]);
        for (u_int64_t i = 1; i < sequence.size() && seq.size() < max_seq_len; i++)
        {
            if (sequence[i] != seq.back())
            {
                seq.emplace_back(sequence[i]);
                // debug
                cout << "(i= "<<i<<")adding to compressed seq: " << sequence[i] << endl;
                continue;
            }
            cout << "(i= "<<i<<")skipped id to add to seq: " << sequence[i] << endl;    // debug
        }
    }
}
